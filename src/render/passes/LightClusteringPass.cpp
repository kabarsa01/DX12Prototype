#include "LightClusteringPass.h"
#include "data/DataManager.h"
#include "../DataStructures.h"
#include "PassBase.h"
#include "render/Renderer.h"
#include "ZPrepass.h"
#include "scene/light/LightComponent.h"
#include "scene/SceneObjectComponent.h"
#include "scene/SceneObjectBase.h"
#include "scene/Transform.h"
#include "utils/HelperUtils.h"
#include "scene/Scene.h"
#include "glm/fwd.hpp"
#include "core/Engine.h"
#include "../PerFrameData.h"
#include "utils/ImageUtils.h"

using namespace glm;

LightClusteringPass::LightClusteringPass(HashString inName)
	: PassBase(inName)
{

}

void LightClusteringPass::RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList)
{
	ScenePtr scene = Engine::GetSceneInstance();
	std::vector<LightComponentPtr> lights = scene->GetSceneComponentsCast<LightComponent>();
	std::map<LightType, std::vector<LightInfo>> sortedLights;
	for (LightComponentPtr lightComp : lights)
	{
		LightInfo info;
		info.direction = vec4(lightComp->GetParent()->transform.GetForwardVector(), 0.0);
		info.position = vec4(lightComp->GetParent()->transform.GetLocation(), 1.0);
		info.color = vec4(lightComp->color, 0.0);
		info.rai.x = lightComp->radius;
		info.rai.y = lightComp->spotHalfAngle;
		info.rai.z = lightComp->intensity;

		sortedLights[lightComp->type].push_back(info);
	}
	lightsIndices->directionalPosition.x = 0;
	lightsIndices->directionalPosition.y = static_cast<uint32_t>(sortedLights[LT_Directional].size());
	lightsIndices->pointPosition.x = lightsIndices->directionalPosition.y;
	lightsIndices->pointPosition.y = static_cast<uint32_t>(sortedLights[LT_Point].size());
	lightsIndices->spotPosition.x = lightsIndices->pointPosition.x + lightsIndices->pointPosition.y;
	lightsIndices->spotPosition.y = static_cast<uint32_t>(sortedLights[LT_Spot].size());

	std::memcpy(&lightsList->lights[lightsIndices->directionalPosition.x], sortedLights[LT_Directional].data(), sizeof(LightInfo) * lightsIndices->directionalPosition.y);
	std::memcpy(&lightsList->lights[lightsIndices->spotPosition.x], sortedLights[LT_Spot].data(), sizeof(LightInfo) * lightsIndices->spotPosition.y);
	std::memcpy(&lightsList->lights[lightsIndices->pointPosition.x], sortedLights[LT_Point].data(), sizeof(LightInfo) * lightsIndices->pointPosition.y);

	computeMaterial->UpdateUniformBuffer<LightsList>("lightsList", *lightsList);
	computeMaterial->UpdateUniformBuffer<LightsIndices>("lightsIndices", *lightsIndices);

	// barriers ----------------------------------------------
	std::array<CD3DX12_RESOURCE_BARRIER, 2> depthBarriers;
	depthBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		depthTexture->GetImage(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	depthBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		depthTexture->GetImage(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_READ
	);
	inCommandList->ResourceBarrier(1, depthBarriers.data());

	PipelineData& pipelineData = FindPipeline(computeMaterial);

	inCommandList->SetPipelineState(pipelineData.pipeline.Get());
	inCommandList->SetComputeRootSignature(pipelineData.rootSignature.Get());
	inCommandList->SetDescriptorHeaps(1, GetRenderer()->GetPerFrameData()->GetDescriptorBlock().heap.GetAddressOf());

	inCommandList->SetComputeRootDescriptorTable(1, GetRenderer()->GetPerFrameData()->GetGPUDescriptorHandle());
	if (computeMaterial->GetDescriptorBlock().heap)
	{
		inCommandList->SetComputeRootDescriptorTable(2, computeMaterial->GetDescriptorBlock().gpuHandle);
	}
	inCommandList->Dispatch(32, 32, 1);

	std::array<CD3DX12_RESOURCE_BARRIER, 2> uavBarriers;
	uavBarriers[0] = CD3DX12_RESOURCE_BARRIER::UAV(computeMaterial->GetStorageBuffer("lightClusters"));
	uavBarriers[1] = CD3DX12_RESOURCE_BARRIER::UAV(computeMaterial->GetStorageBuffer("lightClustersIndices"));

	inCommandList->ResourceBarrier(static_cast<UINT>(uavBarriers.size()), uavBarriers.data());
	inCommandList->ResourceBarrier(1, &depthBarriers[1]);
}

void LightClusteringPass::OnCreate()
{
	ZPrepass* zPrepass = GetRenderer()->GetZPrepass();
	depthTexture = ObjectBase::NewObject<Texture2D, const HashString&>("ComputeDepthTexture");
	depthTexture->CreateFromExternal(zPrepass->GetDepthAttachment(), false);

	debugClustersTexture = ObjectBase::NewObject<Texture2D, const HashString&>("ComputeDebugClustersTexture");
	debugClustersTexture->CreateFromExternal(ImageUtils::CreateTexture2D(GetDevice(), GetWidth(), GetHeight(), true), true);

	lightClusters = new LightClusters();
	lightClustersIndices = new LightClustersIndices();
	lightsList = new LightsList();
	lightsIndices = new LightsIndices();

	computeMaterial = DataManager::RequestResourceType<Material>("LightClusteringMaterial");
	computeMaterial->SetComputeShaderPath("content/shaders/LightClustering.hlsl");
	computeMaterial->SetStorageBuffer<LightClusters>("lightClusters", *lightClusters);
	computeMaterial->SetStorageBuffer<LightClustersIndices>("lightClustersIndices", *lightClustersIndices);
	computeMaterial->SetUniformBuffer<LightsList>("lightsList", *lightsList);
	computeMaterial->SetUniformBuffer<LightsIndices>("lightsIndices", *lightsIndices);
	computeMaterial->SetTexture("depthTexture", depthTexture);
	computeMaterial->SetStorageTexture("debugClustersTexture", debugClustersTexture);
	computeMaterial->LoadResources();
}

void LightClusteringPass::OnDestroy()
{
	delete lightClusters;
	delete lightClustersIndices;
	delete lightsList;
	delete lightsIndices;
}

void LightClusteringPass::CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews)
{
}

void LightClusteringPass::CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight)
{
}

ImageResource LightClusteringPass::CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight)
{
	return {};
}

ResourceView LightClusteringPass::CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock)
{
	return {};
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> LightClusteringPass::CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
	computeDesc.NodeMask = 0;
	computeDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	computeDesc.CS = inMaterial->GetComputeShader()->GetBytecode();
	computeDesc.pRootSignature = inRootSignature.Get();

	ComPtr<ID3D12PipelineState> pipelineState;
	ThrowIfFailed( GetDevice()->GetNativeDevice()->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&pipelineState)) );

	return pipelineState;
}


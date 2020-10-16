#include "PassBase.h"
#include "../Renderer.h"
#include "core/Engine.h"
#include "../shader/Shader.h"
#include "scene/camera/CameraComponent.h"
#include "scene/mesh/MeshComponent.h"
#include "scene/SceneObjectComponent.h"
#include "scene/SceneObjectBase.h"
#include "../DataStructures.h"
#include "scene/Transform.h"
#include "data/DataManager.h"
#include "../PerFrameData.h"
#include "utils/ImageUtils.h"
#include "utils/HelperUtils.h"
#include "../GlobalSamplers.h"

PassBase::PassBase(HashString inName)
	: name(inName)
{

}

PassBase::PassBase()
{

}

PassBase::~PassBase()
{

}

void PassBase::Create()
{
	renderer = Engine::GetRendererInstance();
	device = &renderer->GetDevice();
	width = renderer->GetWidth();
	height = renderer->GetHeight();

	CreateColorAttachments(attachments, width, height);
	if (!isDepthExternal)
	{
		depthAttachment = CreateDepthAttachment(width, height);
	}
	uint32_t descriptorsCount = static_cast<uint32_t>(attachments.size());

	if (attachments.size() > 0)
	{
		rtvViews = Engine::GetRendererInstance()->GetDescriptorHeaps().AllocateDescriptorsRTV(static_cast<uint16_t>(attachments.size()));
		CreateColorAttachmentViews(attachments, rtvViews, attachmentViews);
	}
	if (depthAttachment)
	{
		dsvViews = Engine::GetRendererInstance()->GetDescriptorHeaps().AllocateDescriptorsDSV(1);
		depthAttachmentView = CreateDepthAttachmentView(depthAttachment, dsvViews);
	}

	OnCreate();
}

void PassBase::Destroy()
{
	OnDestroy();

	PipelineRegistry::GetInstance()->DestroyPipelines(device, name);
	for (uint32_t index = 0; index < attachments.size(); index++)
	{
		attachments[index].Destroy();
	}
	if (!isDepthExternal)
	{
		depthAttachment.Destroy();
	}

	DescriptorHeaps::ReleaseDescriptors(rtvViews);
	DescriptorHeaps::ReleaseDescriptors(dsvViews);
}

void PassBase::SetResolution(uint32_t inWidth, uint32_t inHeight)
{
	width = inWidth;
	height = inHeight;
}

void PassBase::SetExternalDepth(const ImageResource& inDepthAttachment)
{
	isDepthExternal = inDepthAttachment;
	if (isDepthExternal)
	{
		depthAttachment = inDepthAttachment;
	}
}

ComPtr<ID3D12RootSignature> PassBase::CreateRootSignature(MaterialPtr inMaterial)
{
	CD3DX12_ROOT_PARAMETER1 rootParams[3];
	rootParams[0].InitAsConstants(1, 0);
	rootParams[1] = renderer->GetPerFrameData()->GetRootParameter();
	rootParams[2] = inMaterial->GetRootParameter();

	GlobalSamplers* samplers = GlobalSamplers::GetInstance();

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootDesc;
	D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootDesc.Init_1_1(3, rootParams, samplers->GetSamplersCounter(), samplers->GetSamplersDescriptions(), flags);
	ComPtr<ID3DBlob> serializedRootSig;
	ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootDesc, &serializedRootSig, &errorBlob));

	ComPtr<ID3D12RootSignature> rootSignature;
	ThrowIfFailed(GetDevice()->GetNativeDevice()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)));

	return rootSignature;
}

PipelineData& PassBase::FindPipeline(MaterialPtr inMaterial)
{
	ComPtr<ID3D12Device2> nativeDevice = device->GetNativeDevice();

	PipelineRegistry& pipelineRegistry = *PipelineRegistry::GetInstance();
	// check pipeline storage and create new pipeline in case it was not created before
	if (!pipelineRegistry.HasPipeline(name, inMaterial->GetShaderHash()))
	{
		PipelineData pipelineData;

		pipelineData.rootSignature = CreateRootSignature(inMaterial);
		pipelineData.pipeline = CreatePipeline(inMaterial, pipelineData.rootSignature);

		pipelineRegistry.StorePipeline(name, inMaterial->GetShaderHash(), pipelineData);
	}

	return pipelineRegistry.GetPipeline(name, inMaterial->GetShaderHash());
}






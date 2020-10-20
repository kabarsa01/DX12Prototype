#include "DeferredLightingPass.h"
#include "utils/ImageUtils.h"
#include "data/MeshData.h"
#include "GBufferPass.h"
#include "data/DataManager.h"
#include "../DataStructures.h"
#include "LightClusteringPass.h"
#include "ZPrepass.h"
#include "PassBase.h"
#include "../Renderer.h"
#include "../PerFrameData.h"
#include "utils/HelperUtils.h"

DeferredLightingPass::DeferredLightingPass(HashString inName)
	: PassBase(inName)
{

}

void DeferredLightingPass::RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList)
{
	LightClusteringPass* clusteringPass = GetRenderer()->GetLightClusteringPass();
	BufferResource& buffer = clusteringPass->computeMaterial->GetStorageBuffer("clusterLightsData");

	// barriers ----------------------------------------------
	std::array<CD3DX12_RESOURCE_BARRIER, 3> preBarriers;
	preBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		depthTexture->GetImage(),
		D3D12_RESOURCE_STATE_DEPTH_READ,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	preBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		albedoTexture->GetImage(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	preBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(
		normalTexture->GetImage(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	inCommandList->ResourceBarrier(static_cast<UINT>(preBarriers.size()), preBarriers.data());
	// barriers ----------------------------------------------

	MeshDataPtr meshData = MeshData::FullscreenQuad();
	PipelineData& pipelineData = FindPipeline(lightingMaterial);

	uint64_t offset = 0;

	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(GetWidth());
	viewport.Height = static_cast<float>(GetHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorsRect;
	scissorsRect.top = 0;
	scissorsRect.left = 0;
	scissorsRect.right = GetWidth();
	scissorsRect.bottom = GetHeight();

	inCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	inCommandList->RSSetViewports(1, &viewport);
	inCommandList->RSSetScissorRects(1, &scissorsRect);

	ResourceView view = GetAttachmentViews()[0];
	inCommandList->OMSetRenderTargets(1, view, TRUE, nullptr);
	inCommandList->SetPipelineState(pipelineData.pipeline.Get());
	inCommandList->SetGraphicsRootSignature(pipelineData.rootSignature.Get());

	std::array<ID3D12DescriptorHeap*, 1> heaps = { GetRenderer()->GetPerFrameData()->GetDescriptorBlock().heap.Get() };
	inCommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());
	inCommandList->SetGraphicsRootDescriptorTable(1, GetRenderer()->GetPerFrameData()->GetGPUDescriptorHandle());
	if (lightingMaterial->GetDescriptorBlock().heap)
	{
		inCommandList->SetGraphicsRootDescriptorTable(2, lightingMaterial->GetDescriptorBlock().gpuHandle);
	}

	HashString meshId = meshData->GetResourceId();

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	inCommandList->ClearRenderTargetView(view, clearColor, 1, &scissorsRect);
	inCommandList->IASetVertexBuffers(0, 1, &meshData->GetVertexBufferView());
	inCommandList->IASetIndexBuffer(&meshData->GetIndexBufferView());
	inCommandList->DrawIndexedInstanced(meshData->GetIndexCount(), 1, 0, 0, 0);

	// barriers ----------------------------------------------
	std::array<CD3DX12_RESOURCE_BARRIER, 3> postBarriers;
	postBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		depthTexture->GetImage(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);
	postBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		albedoTexture->GetImage(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	postBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(
		normalTexture->GetImage(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	inCommandList->ResourceBarrier(static_cast<UINT>(postBarriers.size()), postBarriers.data());
	// barriers ----------------------------------------------
}

void DeferredLightingPass::OnCreate()
{
	ZPrepass* zPrepass = GetRenderer()->GetZPrepass();
	GBufferPass* gBufferPass = GetRenderer()->GetGBufferPass();
	LightClusteringPass* clusteringPass = GetRenderer()->GetLightClusteringPass();

	albedoTexture = ObjectBase::NewObject<Texture2D, const HashString&>("DeferredLightingAlbedoTexture");
	albedoTexture->CreateFromExternal(gBufferPass->GetAttachments()[0], false);
	normalTexture = ObjectBase::NewObject<Texture2D, const HashString&>("DeferredLightingNormalTexture");
	normalTexture->CreateFromExternal(gBufferPass->GetAttachments()[1], false);
	depthTexture = ObjectBase::NewObject<Texture2D, const HashString&>("DeferredLightingDepthTexture");
	depthTexture->CreateFromExternal(zPrepass->GetDepthAttachment(), false);
	clusterDebugTexture = ObjectBase::NewObject<Texture2D, const HashString&>("DeferredLightingClustersTexture");
	clusterDebugTexture->CreateFromExternal(clusteringPass->debugClustersTexture->GetImage(), false);

	lightingMaterial = DataManager::RequestResourceType<Material, const std::string&, const std::string&>(
		"DeferredLightingMaterial",
		"content/shaders/DeferredLighting.hlsl",
		"content/shaders/DeferredLighting.hlsl"
		);
	lightingMaterial->SetEntrypoints("VSmain", "PSmain");
	lightingMaterial->SetStorageBufferExternal("lightClusters", clusteringPass->computeMaterial->GetStorageBuffer("lightClusters"));
	lightingMaterial->SetStorageBufferExternal("lightClustersIndices", clusteringPass->computeMaterial->GetStorageBuffer("lightClustersIndices"));
	lightingMaterial->SetUniformBufferExternal("lightsList", clusteringPass->computeMaterial->GetUniformBuffer("lightsList"));
	lightingMaterial->SetUniformBufferExternal("lightsIndices", clusteringPass->computeMaterial->GetUniformBuffer("lightsIndices"));
	lightingMaterial->SetTexture("albedoTex", albedoTexture);
	lightingMaterial->SetTexture("normalsTex", normalTexture);
	lightingMaterial->SetTexture("depthTex", depthTexture);
	lightingMaterial->SetTexture("debugClustersTexture", clusterDebugTexture);
	lightingMaterial->LoadResources();
}

void DeferredLightingPass::CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight)
{
	outAttachments.push_back(ImageUtils::CreateColorAttachment(GetDevice(), inWidth, inHeight, true));
}

void DeferredLightingPass::CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews)
{
	outAttachmentViews.push_back(ResourceView::CreateRTVTexture2D(inAttachments[0], inBlock, 0));
}

ImageResource DeferredLightingPass::CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight)
{
	return {};
}

ResourceView DeferredLightingPass::CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock)
{
	return {};
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> DeferredLightingPass::CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature)
{
	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	std::array<D3D12_INPUT_ELEMENT_DESC, 5> attributes = Vertex::GetAttributeDescriptions();
	D3D12_INPUT_LAYOUT_DESC inputLayout;
	inputLayout.NumElements = static_cast<UINT>(attributes.size());
	inputLayout.pInputElementDescs = attributes.data();

	D3D12_DEPTH_STENCILOP_DESC depthStencilOp;
	depthStencilOp.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOp.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOp.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilOp.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	CD3DX12_DEPTH_STENCIL_DESC depthState(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.BackFace = depthStencilOp;
	depthDesc.DepthEnable = FALSE;
	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthDesc.FrontFace = depthStencilOp;
	depthDesc.StencilEnable = FALSE;

	D3D12_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.VS = inMaterial->GetVertexShader()->GetBytecode();
	psoDesc.PS = inMaterial->GetFragmentShader()->GetBytecode();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = depthDesc;//CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;

	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
#if defined(DEBUG) || defined(_DEBUG)
	//	psoDesc.Flags |= D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
#endif

	psoDesc.InputLayout = inputLayout;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.SampleDesc = sampleDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.pRootSignature = inRootSignature.Get();

	ComPtr<ID3D12PipelineState> pipelineState;
	ThrowIfFailed(GetDevice()->GetNativeDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

	return pipelineState;
}


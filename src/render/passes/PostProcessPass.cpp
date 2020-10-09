#include "PostProcessPass.h"
#include "../Renderer.h"
#include "data/DataManager.h"
#include "GBufferPass.h"
#include "../DataStructures.h"
#include "DeferredLightingPass.h"
#include "LightClusteringPass.h"
#include "data/MeshData.h"
#include "PassBase.h"
#include "../PerFrameData.h"
#include "utils/HelperUtils.h"

PostProcessPass::PostProcessPass(HashString inName)
	: PassBase(inName)
{

}

void PostProcessPass::RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList)
{
	SwapChain& swapChain = GetRenderer()->GetSwapChain();
	ComPtr<ID3D12Resource> swapChainImage = swapChain.GetCurrentImage();

	// barriers ----------------------------------------------
	//ImageMemoryBarrier attachmentBarrier;
	//attachmentBarrier.setImage(swapChainImage);
	//attachmentBarrier.setOldLayout(ImageLayout::eUndefined);
	//attachmentBarrier.setNewLayout(ImageLayout::eColorAttachmentOptimal);
	//attachmentBarrier.subresourceRange.setAspectMask(ImageAspectFlagBits::eColor);
	//attachmentBarrier.subresourceRange.setBaseMipLevel(0);
	//attachmentBarrier.subresourceRange.setLevelCount(1);
	//attachmentBarrier.subresourceRange.setBaseArrayLayer(0);
	//attachmentBarrier.subresourceRange.setLayerCount(1);
	//attachmentBarrier.setSrcAccessMask(AccessFlagBits::eMemoryRead);
	//attachmentBarrier.setDstAccessMask(AccessFlagBits::eMemoryWrite);
	//inCommandBuffer->pipelineBarrier(
	//	PipelineStageFlagBits::eVertexShader,
	//	PipelineStageFlagBits::eFragmentShader,
	//	DependencyFlags(),
	//	0, nullptr, 0, nullptr,
	//	1, &attachmentBarrier);

	MeshDataPtr meshData = MeshData::FullscreenQuad();

	PipelineData& pipelineData = FindPipeline(postProcessMaterial);

	//ClearValue clearValue;
	//clearValue.setColor(ClearColorValue(std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 1.0f })));

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

	DescriptorBlock& descBlock = GetRtvBlock();
	inCommandList->OMSetRenderTargets(descBlock.size, &descBlock.cpuHandle, TRUE, &GetDsvBlock().cpuHandle);
	inCommandList->SetPipelineState(pipelineData.pipeline.Get());
	inCommandList->SetGraphicsRootSignature(pipelineData.rootSignature.Get());

	std::array<ID3D12DescriptorHeap*, 3> heaps = { GetRtvBlock().heap.Get(), GetDsvBlock().heap.Get(), postProcessMaterial->GetDescriptorBlock().heap.Get() };
	inCommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());
	inCommandList->SetGraphicsRootDescriptorTable(1, GetRenderer()->GetPerFrameData()->GetGPUDescriptorHandle());
	inCommandList->SetGraphicsRootDescriptorTable(2, postProcessMaterial->GetDescriptorBlock().gpuHandle);

	HashString meshId = meshData->GetResourceId();

//	inCommandList->SetGraphicsRoot32BitConstant(0, meshData, 0);
	inCommandList->IASetVertexBuffers(0, 1, &meshData->GetVertexBufferView());
	inCommandList->IASetIndexBuffer(&meshData->GetIndexBufferView());
	//				inCommandList->pushConstants(pipelineData.pipelineLayout, ShaderStageFlagBits::eAllGraphics, 0, sizeof(uint32_t), & scene->GetMeshDataToIndex(materialId)[meshId]);
	//				inCommandList->bindVertexBuffers(0, 1, &meshData->GetVertexBuffer().GetBuffer(), &offset);
	//				inCommandList->bindIndexBuffer(meshData->GetIndexBuffer().GetBuffer(), 0, IndexType::eUint32);
	inCommandList->DrawIndexedInstanced(meshData->GetIndexCount(), 1, 0, 0, 0);

	//ImageMemoryBarrier presentBarrier;
	//presentBarrier.setImage(swapChainImage);
	//presentBarrier.setOldLayout(ImageLayout::eColorAttachmentOptimal);
	//presentBarrier.setNewLayout(ImageLayout::ePresentSrcKHR);
	//presentBarrier.subresourceRange.setAspectMask(ImageAspectFlagBits::eColor);
	//presentBarrier.subresourceRange.setBaseMipLevel(0);
	//presentBarrier.subresourceRange.setLevelCount(1);
	//presentBarrier.subresourceRange.setBaseArrayLayer(0);
	//presentBarrier.subresourceRange.setLayerCount(1);
	//presentBarrier.setSrcAccessMask(AccessFlagBits::eMemoryWrite);
	//presentBarrier.setDstAccessMask(AccessFlagBits::eMemoryRead);
	//inCommandList->pipelineBarrier(
	//	PipelineStageFlagBits::eFragmentShader,
	//	PipelineStageFlagBits::eHost,
	//	DependencyFlags(),
	//	0, nullptr, 0, nullptr,
	//	1, &presentBarrier);
}

void PostProcessPass::OnCreate()
{
	//DeferredLightingPass* lightingPass = GetRenderer()->GetDeferredLightingPass();
	//screenImage = ObjectBase::NewObject<Texture2D, const HashString&>("SceneImageTexture");
	//screenImage->CreateFromExternal(lightingPass->GetAttachments()[0]);
	screenImage = DataManager::RequestResourceType<Texture2D, bool, bool, bool, bool>("content/meshes/root/Aset_wood_root_M_rkswd_4K_Albedo.jpg", false, true, false, true);

	postProcessMaterial = DataManager::RequestResourceType<Material, const std::string&, const std::string&>(
		"PostProcessMaterial",
		"content/shaders/PostProcessVert.spv",
		"content/shaders/PostProcessFrag.spv"
		);
	ObjectMVPData objData;
	postProcessMaterial->SetUniformBuffer<ObjectMVPData>("mvpBuffer", objData);
	postProcessMaterial->SetTexture("screenImage", screenImage);//GetRenderer()->GetLightClusteringPass()->texture);
	postProcessMaterial->LoadResources();
}

void PostProcessPass::CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight)
{
}

ImageResource PostProcessPass::CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight)
{
	return ImageResource();
}

ResourceView PostProcessPass::CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock)
{
	return ResourceView();
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PostProcessPass::CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature)
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
	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthDesc.FrontFace = depthStencilOp;
	depthDesc.StencilEnable = FALSE;

	D3D12_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	psoDesc.VS = inMaterial->GetVertexShader()->GetBytecode();
	psoDesc.PS = inMaterial->GetFragmentShader()->GetBytecode();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = depthDesc;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;

	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
#if defined(DEBUG) || defined(_DEBUG)
	psoDesc.Flags |= D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
#endif

	psoDesc.InputLayout = inputLayout;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.SampleDesc = sampleDesc;
	psoDesc.SampleMask = 0;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.pRootSignature = inRootSignature.Get();

	ComPtr<ID3D12PipelineState> pipelineState;
	ThrowIfFailed(GetDevice()->GetNativeDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

	return pipelineState;
}

void PostProcessPass::CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews)
{
}


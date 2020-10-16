#include "ZPrepass.h"
#include "utils/ImageUtils.h"
#include "data/MeshData.h"
#include "scene/mesh/MeshComponent.h"
#include "utils/HelperUtils.h"
#include "PassBase.h"
#include "../Renderer.h"
#include "../PerFrameData.h"

ZPrepass::ZPrepass(HashString inName)
	: PassBase(inName)
{

}

void ZPrepass::RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList)
{
	//ImageMemoryBarrier depthTextureBarrier = GetDepthAttachment().CreateLayoutBarrier(
//	ImageLayout::eUndefined,
//	ImageLayout::eDepthAttachmentOptimal,
//	AccessFlagBits::eShaderRead,
//	AccessFlagBits::eDepthStencilAttachmentRead,
//	ImageAspectFlagBits::eDepth | ImageAspectFlagBits::eStencil,
//	0, 1, 0, 1);
//inCommandList->pipelineBarrier(
//	PipelineStageFlagBits::eComputeShader,
//	PipelineStageFlagBits::eAllGraphics,
//	DependencyFlags(),
//	0, nullptr, 0, nullptr,
//	1, &depthTextureBarrier);

	ScenePtr scene = Engine::GetSceneInstance();

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

	inCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	inCommandList->RSSetViewports(1, &viewport);
	inCommandList->RSSetScissorRects(1, &scissorsRect);

	inCommandList->OMSetRenderTargets(0, nullptr, FALSE, &GetDsvBlock().cpuHandle);
	inCommandList->ClearDepthStencilView(GetDepthAttachmentView(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &scissorsRect);

	//------------------------------------------------------------------------------------------------------------
	for (HashString& shaderHash : scene->GetShadersList())
	{
		PipelineData& pipelineData = FindPipeline(scene->GetShaderToMaterial()[shaderHash][0]);

		inCommandList->SetPipelineState(pipelineData.pipeline.Get());
		inCommandList->SetGraphicsRootSignature(pipelineData.rootSignature.Get());

		for (MaterialPtr material : scene->GetShaderToMaterial()[shaderHash])
		{
			HashString materialId = material->GetResourceId();

			std::array<ID3D12DescriptorHeap*, 1> heaps = { GetRenderer()->GetPerFrameData()->GetDescriptorBlock().heap.Get() };
			inCommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());
			inCommandList->SetGraphicsRootDescriptorTable(1, GetRenderer()->GetPerFrameData()->GetGPUDescriptorHandle());
			if (material->GetDescriptorBlock().heap)
			{
				inCommandList->SetGraphicsRootDescriptorTable(2, material->GetDescriptorBlock().gpuHandle);
			}

			for (MeshDataPtr meshData : scene->GetMaterialToMeshData()[material->GetResourceId()])
			{
				HashString meshId = meshData->GetResourceId();

				inCommandList->SetGraphicsRoot32BitConstant(0, scene->GetMeshDataToIndex(materialId)[meshId], 0);
				inCommandList->IASetVertexBuffers(0, 1, &meshData->GetVertexBufferView());
				inCommandList->IASetIndexBuffer(&meshData->GetIndexBufferView());
				//				inCommandList->pushConstants(pipelineData.pipelineLayout, ShaderStageFlagBits::eAllGraphics, 0, sizeof(uint32_t), & scene->GetMeshDataToIndex(materialId)[meshId]);
				//				inCommandList->bindVertexBuffers(0, 1, &meshData->GetVertexBuffer().GetBuffer(), &offset);
				//				inCommandList->bindIndexBuffer(meshData->GetIndexBuffer().GetBuffer(), 0, IndexType::eUint32);
				inCommandList->DrawIndexedInstanced(
					static_cast<UINT>(meshData->GetIndexCount()),
					static_cast<UINT>(scene->GetMeshDataToTransform(materialId)[meshId].size()),
					0,
					0,
					0);
			}
		}
	}
}

void ZPrepass::OnCreate()
{
}

void ZPrepass::CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews)
{
}

void ZPrepass::CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight)
{
}

ImageResource ZPrepass::CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight)
{
	return ImageUtils::CreateDepthAttachment(GetDevice(), inWidth, inHeight);
}

ResourceView ZPrepass::CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock)
{
	return ResourceView::CreateDSVTexture2D(inDepthAttachment, inBlock, 0);
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ZPrepass::CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature)
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

	D3D12_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.BackFace = depthStencilOp;
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.VS = inMaterial->GetVertexShader()->GetBytecode();
//	psoDesc.PS = inMaterial->GetFragmentShader()->GetBytecode();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = depthDesc;
	psoDesc.NumRenderTargets = 0;
	//psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
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


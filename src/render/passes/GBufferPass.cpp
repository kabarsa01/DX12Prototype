#include "GBufferPass.h"
#include "utils/ImageUtils.h"
#include "data/MeshData.h"
#include "scene/mesh/MeshComponent.h"
#include "d3dx12.h"
#include "utils/HelperUtils.h"
#include "../objects/DescriptorPool.h"

GBufferPass::GBufferPass(HashString inName)
	:PassBase(inName)
{
}

void GBufferPass::RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList)
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

	//RenderPassBeginInfo passBeginInfo;
	//passBeginInfo.setRenderPass(GetRenderPass());
	//passBeginInfo.setFramebuffer(GetFramebuffer());
	//passBeginInfo.setRenderArea(Rect2D(Offset2D(0, 0), Extent2D(GetWidth(), GetHeight())));
	//passBeginInfo.setClearValueCount(static_cast<uint32_t>( clearValues.size() ));
	//passBeginInfo.setPClearValues(clearValues.data());

	//DeviceSize offset = 0;
	//inCommandList->beginRenderPass(passBeginInfo, SubpassContents::eInline);

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
	inCommandList->OMSetRenderTargets(descBlock.size, & descBlock.cpuHandle, TRUE, & GetDsvBlock().cpuHandle);
//	inCommandList->ClearRenderTargetView(

	//------------------------------------------------------------------------------------------------------------
	for (HashString& shaderHash : scene->GetShadersList())
	{
		PipelineData& pipelineData = FindPipeline(scene->GetShaderToMaterial()[shaderHash][0]);

		inCommandList->SetPipelineState(pipelineData.pipeline.Get());
		inCommandList->SetGraphicsRootSignature(pipelineData.rootSignature.Get());
		//inCommandList->bindPipeline(PipelineBindPoint::eGraphics, pipelineData.pipeline);
		//inCommandList->bindDescriptorSets(PipelineBindPoint::eGraphics, pipelineData.pipelineLayout, 0, pipelineData.descriptorSets, {});

		for (MaterialPtr material : scene->GetShaderToMaterial()[shaderHash])
		{
			HashString materialId = material->GetResourceId();

//			material->CreateDescriptorSet(GetVulkanDevice());
			std::array<ID3D12DescriptorHeap*, 3> heaps = { GetRtvBlock().heap.Get(), GetDsvBlock().heap.Get(), material->GetDescriptorBlock().heap.Get() };
			inCommandList->SetDescriptorHeaps(heaps.size(), heaps.data());
			inCommandList->SetGraphicsRootDescriptorTable(2, material->GetDescriptorBlock().gpuHandle);
//			inCommandList->bindDescriptorSets(PipelineBindPoint::eGraphics, pipelineData.pipelineLayout, 1, material->GetDescriptorSets(), {});

			for (MeshDataPtr meshData : scene->GetMaterialToMeshData()[material->GetResourceId()])
			{
				HashString meshId = meshData->GetResourceId();

				inCommandList->SetGraphicsRoot32BitConstant(0, scene->GetMeshDataToIndex(materialId)[meshId], 0);
				inCommandList->IASetVertexBuffers(0, 1, & meshData->GetVertexBufferView());
				inCommandList->IASetIndexBuffer(& meshData->GetIndexBufferView());
//				inCommandList->pushConstants(pipelineData.pipelineLayout, ShaderStageFlagBits::eAllGraphics, 0, sizeof(uint32_t), & scene->GetMeshDataToIndex(materialId)[meshId]);
//				inCommandList->bindVertexBuffers(0, 1, &meshData->GetVertexBuffer().GetBuffer(), &offset);
//				inCommandList->bindIndexBuffer(meshData->GetIndexBuffer().GetBuffer(), 0, IndexType::eUint32);
				inCommandList->DrawIndexedInstanced(meshData->GetIndexCount(), scene->GetMeshDataToTransform(materialId)[meshId].size(), 0, 0, 0);
			}
		}		
	}
	//------------------------------------------------------------------------------------------------------------
//	inCommandList->endRenderPass();
}

void GBufferPass::OnCreate()
{
	//// just init clear values
	//clearValues[0].setColor(ClearColorValue(std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 1.0f })));
	//clearValues[1].setColor(ClearColorValue(std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 1.0f })));
	//clearValues[2].setDepthStencil(ClearDepthStencilValue(1.0f, 0));
}
//
//RenderPass GBufferPass::CreateRenderPass()
//{
//	// albedo
//	AttachmentDescription albedoAttachDesc;
//	albedoAttachDesc.setFormat(Format::eR8G8B8A8Unorm);
//	albedoAttachDesc.setSamples(SampleCountFlagBits::e1);
//	albedoAttachDesc.setLoadOp(AttachmentLoadOp::eClear);
//	albedoAttachDesc.setStoreOp(AttachmentStoreOp::eStore);
//	albedoAttachDesc.setStencilLoadOp(AttachmentLoadOp::eDontCare);
//	albedoAttachDesc.setStencilStoreOp(AttachmentStoreOp::eDontCare);
//	albedoAttachDesc.setInitialLayout(ImageLayout::eUndefined);
//	albedoAttachDesc.setFinalLayout(ImageLayout::eColorAttachmentOptimal);
//	AttachmentReference albedoAttachRef;
//	albedoAttachRef.setAttachment(0);
//	albedoAttachRef.setLayout(ImageLayout::eColorAttachmentOptimal);
//	// normal
//	AttachmentDescription normalAttachDesc;
//	normalAttachDesc.setFormat(Format::eR16G16B16A16Sfloat);
//	normalAttachDesc.setSamples(SampleCountFlagBits::e1);
//	normalAttachDesc.setLoadOp(AttachmentLoadOp::eClear);
//	normalAttachDesc.setStoreOp(AttachmentStoreOp::eStore);
//	normalAttachDesc.setStencilLoadOp(AttachmentLoadOp::eDontCare);
//	normalAttachDesc.setStencilStoreOp(AttachmentStoreOp::eDontCare);
//	normalAttachDesc.setInitialLayout(ImageLayout::eUndefined);
//	normalAttachDesc.setFinalLayout(ImageLayout::eColorAttachmentOptimal);
//	AttachmentReference normalAttachRef;
//	normalAttachRef.setAttachment(1);
//	normalAttachRef.setLayout(ImageLayout::eColorAttachmentOptimal);
//
//	// depth
//	AttachmentDescription depthAttachDesc;
//	depthAttachDesc.setFormat(Format::eD24UnormS8Uint);
//	depthAttachDesc.setSamples(SampleCountFlagBits::e1);
//	depthAttachDesc.setLoadOp(AttachmentLoadOp::eLoad);
//	depthAttachDesc.setStoreOp(AttachmentStoreOp::eStore);
//	depthAttachDesc.setStencilLoadOp(AttachmentLoadOp::eLoad);
//	depthAttachDesc.setStencilStoreOp(AttachmentStoreOp::eStore);
//	depthAttachDesc.setInitialLayout(ImageLayout::eUndefined);
//	depthAttachDesc.setFinalLayout(ImageLayout::eDepthStencilAttachmentOptimal);
//	AttachmentReference depthAttachRef;
//	depthAttachRef.setAttachment(2);
//	depthAttachRef.setLayout(ImageLayout::eDepthStencilAttachmentOptimal);
//
//	std::array<AttachmentReference, 2> colorAttachments{ albedoAttachRef, normalAttachRef };
//	SubpassDescription subpassDesc;
//	subpassDesc.setColorAttachmentCount(static_cast<uint32_t>(colorAttachments.size()));
//	subpassDesc.setPColorAttachments(colorAttachments.data());
//	subpassDesc.setPDepthStencilAttachment(&depthAttachRef);
//	subpassDesc.setPipelineBindPoint(PipelineBindPoint::eGraphics);
//
//	SubpassDependency subpassDependency;
//	subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
//	subpassDependency.setDstSubpass(0);
//	subpassDependency.setSrcStageMask(PipelineStageFlagBits::eColorAttachmentOutput);
//	subpassDependency.setSrcAccessMask(AccessFlags());
//	subpassDependency.setDstStageMask(PipelineStageFlagBits::eColorAttachmentOutput);
//	subpassDependency.setDstAccessMask(AccessFlagBits::eColorAttachmentRead | AccessFlagBits::eColorAttachmentWrite);
//
//	std::array<AttachmentDescription, 3> attachDescriptions = { albedoAttachDesc, normalAttachDesc, depthAttachDesc };
//	RenderPassCreateInfo renderPassInfo;
//	renderPassInfo.setAttachmentCount(static_cast<uint32_t>(attachDescriptions.size()));
//	renderPassInfo.setPAttachments(attachDescriptions.data());
//	renderPassInfo.setSubpassCount(1);
//	renderPassInfo.setPSubpasses(&subpassDesc);
//	renderPassInfo.setDependencyCount(1);
//	renderPassInfo.setPDependencies(&subpassDependency);
//
//	return GetVulkanDevice()->GetDevice().createRenderPass(renderPassInfo);
//}

void GBufferPass::CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight)
{
	Device* device = GetDevice();

	// albedo
	ImageResource albedoAttachmentImage = ImageUtils::CreateColorAttachment(device, inWidth, inHeight);
	outAttachments.push_back(albedoAttachmentImage);
	// normal
	ImageResource normalAttachmentImage = ImageUtils::CreateColorAttachment(device, inWidth, inHeight, true);
	outAttachments.push_back(normalAttachmentImage);
//	outAttachmentViews.push_back(normalAttachmentImage.CreateView({ ImageAspectFlagBits::eColor, 0, 1, 0, 1 }, ImageViewType::e2D));
}

void GBufferPass::CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews)
{
	for (uint32_t index = 0; index < inAttachments.size(); index++)
	{
		outAttachmentViews.push_back(ResourceView::CreateSRVTexture2D(inAttachments[index], inBlock, index));
	}
}

ImageResource GBufferPass::CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight)
{
	return ImageUtils::CreateDepthAttachment(GetDevice(), inWidth, inHeight);
}

ResourceView GBufferPass::CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock)
{
	return ResourceView::CreateDSVTexture2D(inDepthAttachment, inBlock, 0);
}

ComPtr<ID3D12PipelineState> GBufferPass::CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature)
{
	//std::vector<PipelineShaderStageCreateInfo> shaderStageInfoArray = { inMaterial->GetVertexStageInfo(), inMaterial->GetFragmentStageInfo() };

	//VertexInputBindingDescription bindingDesc = MeshData::GetBindingDescription(0);
	//std::array<VertexInputAttributeDescription, 5> attributeDesc = Vertex::GetAttributeDescriptions(0);
	//PipelineVertexInputStateCreateInfo vertexInputInfo;
	//vertexInputInfo.setVertexBindingDescriptionCount(1);
	//vertexInputInfo.setPVertexBindingDescriptions(&bindingDesc);
	//vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDesc.size()));
	//vertexInputInfo.setPVertexAttributeDescriptions(attributeDesc.data());

	//PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	//inputAssemblyInfo.setTopology(PrimitiveTopology::eTriangleList);
	//inputAssemblyInfo.setPrimitiveRestartEnable(VK_FALSE);

	//Viewport viewport;
	//viewport.setX(0.0f);
	//viewport.setY(0.0f);
	//viewport.setWidth(static_cast<float>(GetWidth()));
	//viewport.setHeight(static_cast<float>(GetHeight()));
	//viewport.setMinDepth(0.0f);
	//viewport.setMaxDepth(1.0f);

	//Rect2D scissor;
	//scissor.setOffset(Offset2D(0, 0));
	//scissor.setExtent(Extent2D{ GetWidth(), GetHeight() });

	//PipelineViewportStateCreateInfo viewportInfo;
	//viewportInfo.setViewportCount(1);
	//viewportInfo.setPViewports(&viewport);
	//viewportInfo.setScissorCount(1);
	//viewportInfo.setPScissors(&scissor);

	//PipelineRasterizationStateCreateInfo rasterizationInfo;
	//rasterizationInfo.setDepthClampEnable(VK_FALSE);
	//rasterizationInfo.setRasterizerDiscardEnable(VK_FALSE);
	//rasterizationInfo.setPolygonMode(PolygonMode::eFill);
	//rasterizationInfo.setLineWidth(1.0f);
	//rasterizationInfo.setCullMode(CullModeFlagBits::eBack);
	//rasterizationInfo.setFrontFace(FrontFace::eClockwise);
	//rasterizationInfo.setDepthBiasEnable(VK_FALSE);

	//PipelineMultisampleStateCreateInfo multisampleInfo;

	//PipelineDepthStencilStateCreateInfo depthStencilInfo;
	//depthStencilInfo.setDepthBoundsTestEnable(VK_FALSE);
	//depthStencilInfo.setDepthCompareOp(CompareOp::eEqual);
	//depthStencilInfo.setDepthTestEnable(VK_TRUE);
	//depthStencilInfo.setDepthWriteEnable(VK_FALSE);
	////depthStencilInfo.setMaxDepthBounds(1.0f);
	////depthStencilInfo.setMinDepthBounds(0.0f);
	//depthStencilInfo.setStencilTestEnable(VK_FALSE);

	//PipelineColorBlendAttachmentState albedoBlendAttachment;
	//albedoBlendAttachment.setColorWriteMask(ColorComponentFlagBits::eR | ColorComponentFlagBits::eG | ColorComponentFlagBits::eB | ColorComponentFlagBits::eA);
	//albedoBlendAttachment.setBlendEnable(VK_FALSE);
	//albedoBlendAttachment.setSrcColorBlendFactor(BlendFactor::eOne);
	//albedoBlendAttachment.setDstColorBlendFactor(BlendFactor::eZero);
	//albedoBlendAttachment.setColorBlendOp(BlendOp::eAdd);
	//albedoBlendAttachment.setSrcAlphaBlendFactor(BlendFactor::eOne);
	//albedoBlendAttachment.setDstAlphaBlendFactor(BlendFactor::eZero);
	//albedoBlendAttachment.setAlphaBlendOp(BlendOp::eAdd);
	//PipelineColorBlendAttachmentState normalBlendAttachment;
	//normalBlendAttachment.setColorWriteMask(ColorComponentFlagBits::eR | ColorComponentFlagBits::eG | ColorComponentFlagBits::eB | ColorComponentFlagBits::eA);
	//normalBlendAttachment.setBlendEnable(VK_FALSE);
	//normalBlendAttachment.setSrcColorBlendFactor(BlendFactor::eOne);
	//normalBlendAttachment.setDstColorBlendFactor(BlendFactor::eZero);
	//normalBlendAttachment.setColorBlendOp(BlendOp::eAdd);
	//normalBlendAttachment.setSrcAlphaBlendFactor(BlendFactor::eOne);
	//normalBlendAttachment.setDstAlphaBlendFactor(BlendFactor::eZero);
	//normalBlendAttachment.setAlphaBlendOp(BlendOp::eAdd);

	//std::array<PipelineColorBlendAttachmentState, 2> colorBlendAttachmentStates{ albedoBlendAttachment, normalBlendAttachment };
	//PipelineColorBlendStateCreateInfo colorBlendInfo;
	//colorBlendInfo.setLogicOpEnable(VK_FALSE);
	//colorBlendInfo.setLogicOp(LogicOp::eCopy);
	//colorBlendInfo.setAttachmentCount(static_cast<uint32_t>( colorBlendAttachmentStates.size() ));
	//colorBlendInfo.setPAttachments(colorBlendAttachmentStates.data());
	//colorBlendInfo.setBlendConstants({ 0.0f, 0.0f, 0.0f, 0.0f });

	//GraphicsPipelineCreateInfo pipelineInfo;
	//pipelineInfo.setStageCount(2);
	//pipelineInfo.setPStages(shaderStageInfoArray.data());
	//pipelineInfo.setPVertexInputState(&vertexInputInfo);
	//pipelineInfo.setPInputAssemblyState(&inputAssemblyInfo);
	//pipelineInfo.setPViewportState(&viewportInfo);
	//pipelineInfo.setPRasterizationState(&rasterizationInfo);
	//pipelineInfo.setPMultisampleState(&multisampleInfo);
	//pipelineInfo.setPDepthStencilState(&depthStencilInfo);
	//pipelineInfo.setPColorBlendState(&colorBlendInfo);
	//pipelineInfo.setPDynamicState(nullptr);
	//pipelineInfo.setLayout(inLayout);
	//pipelineInfo.setRenderPass(inRenderPass);
	//pipelineInfo.setSubpass(0);
	//pipelineInfo.setBasePipelineHandle(Pipeline());
	//pipelineInfo.setBasePipelineIndex(-1);

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	std::array<D3D12_INPUT_ELEMENT_DESC, 5> attributes = Vertex::GetAttributeDescriptions();
	D3D12_INPUT_LAYOUT_DESC inputLayout;
	inputLayout.NumElements = attributes.size();
	inputLayout.pInputElementDescs = attributes.data();

	D3D12_DEPTH_STENCILOP_DESC depthStencilOp;
	depthStencilOp.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOp.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOp.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilOp.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	CD3DX12_DEPTH_STENCIL_DESC depthState(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.BackFace = depthStencilOp;
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
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
	psoDesc.NumRenderTargets = 3;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	ThrowIfFailed( GetDevice()->GetNativeDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)) );

	return pipelineState;
}



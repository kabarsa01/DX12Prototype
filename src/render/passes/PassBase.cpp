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

//	renderPass = CreateRenderPass();
//	if (renderPass)
//	{
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
////		std::vector<ImageView> views = attachmentViews;
//		if (depthAttachmentView)
//		{
////			views.push_back(depthAttachmentView);
//		}
////		framebuffer = CreateFramebuffer(renderPass, views, width, height);
//	}

	OnCreate();
}

void PassBase::Destroy()
{
	OnDestroy();

//	Device& device = device->GetNativeDevice();

	PipelineRegistry::GetInstance()->DestroyPipelines(device, name);

	//device.destroyRenderPass(renderPass);
	//device.destroyFramebuffer(framebuffer);
	for (uint32_t index = 0; index < attachments.size(); index++)
	{
//		device.destroyImageView(attachmentViews[index]);
		attachments[index].Destroy();
	}
	if (!isDepthExternal)
	{
//		device.destroyImageView(depthAttachmentView);
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

void PassBase::SetExternalDepth(const ImageResource& inDepthAttachment/*const ImageView& inDepthAttachmentView*/)
{
	isDepthExternal = inDepthAttachment;//&& inDepthAttachmentView;
	if (isDepthExternal)
	{
		depthAttachment = inDepthAttachment;
//		depthAttachmentView = inDepthAttachmentView;
	}
}

//Framebuffer PassBase::CreateFramebuffer(RenderPass inRenderPass, std::vector<ImageView>& inAttachmentViews, uint32_t inWidth, uint32_t inHeight)
//{
//	FramebufferCreateInfo framebufferInfo;
//	framebufferInfo.setRenderPass(inRenderPass);
//	framebufferInfo.setAttachmentCount(static_cast<uint32_t>( inAttachmentViews.size() ));
//	framebufferInfo.setPAttachments(inAttachmentViews.data());
//	framebufferInfo.setWidth(inWidth);
//	framebufferInfo.setHeight(inHeight);
//	framebufferInfo.setLayers(1);
//
//	return vulkanDevice->GetDevice().createFramebuffer(framebufferInfo);
//}

ComPtr<ID3D12RootSignature> PassBase::CreateRootSignature(MaterialPtr inMaterial)// std::vector<DescriptorSetLayout>& inDescriptorSetLayouts)
{
//	Device& device = vulkanDevice->GetDevice();

	//PipelineLayoutCreateInfo pipelineLayoutInfo;
	//pipelineLayoutInfo.setFlags(PipelineLayoutCreateFlags());
	//pipelineLayoutInfo.setSetLayoutCount(static_cast<uint32_t>( inDescriptorSetLayouts.size() ));
	//pipelineLayoutInfo.setPSetLayouts(inDescriptorSetLayouts.data());

	//PushConstantRange pushConstRange;
	//pushConstRange.setOffset(0);
	//pushConstRange.setSize(sizeof(uint32_t));
	//pushConstRange.setStageFlags(ShaderStageFlagBits::eAllGraphics);

	//pipelineLayoutInfo.setPushConstantRangeCount(1);
	//pipelineLayoutInfo.setPPushConstantRanges(&pushConstRange);

	CD3DX12_ROOT_PARAMETER1 rootParams[3];
	rootParams[0].InitAsConstants(1, 0);
	rootParams[1] = renderer->GetPerFrameData()->GetRootParameter();
	rootParams[2] = inMaterial->GetRootParameter();

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootDesc;
	GlobalSamplers* samplers = GlobalSamplers::GetInstance();
	rootDesc.Init_1_1(3, rootParams, samplers->GetSamplersCounter(), samplers->GetSamplersDescriptions(), D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
	ComPtr<ID3DBlob> serializedRootSig;
	ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootDesc, &serializedRootSig, nullptr));

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

//		inMaterial->CreateDescriptorSet(vulkanDevice);
		// TODO get two tables to be used in signature
//		pipelineData.descriptorSets = { renderer->GetPerFrameData()->GetSet(), inMaterial->GetDescriptorSet() };

//		std::vector<DescriptorSetLayout> setLayouts = { renderer->GetPerFrameData()->GetLayout(), inMaterial->GetDescriptorSetLayout() };
		pipelineData.rootSignature = CreateRootSignature(inMaterial);
		pipelineData.pipeline = CreatePipeline(inMaterial, pipelineData.rootSignature);

		pipelineRegistry.StorePipeline(name, inMaterial->GetShaderHash(), pipelineData);
	}

	return pipelineRegistry.GetPipeline(name, inMaterial->GetShaderHash());
}

//DescriptorSetLayout PassBase::CreateDescriptorSetLayout(MaterialPtr inMaterial)
//{
//	DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
//	descriptorSetLayoutInfo.setBindingCount(static_cast<uint32_t>(inMaterial->GetBindings().size()));
//	descriptorSetLayoutInfo.setPBindings(inMaterial->GetBindings().data());
//
//	return vulkanDevice->GetDevice().createDescriptorSetLayout(descriptorSetLayoutInfo);
//}




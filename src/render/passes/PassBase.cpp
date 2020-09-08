#include "PassBase.h"
#include "../Renderer.h"
#include "core/Engine.h"
#include "../shader/Shader.h"
#include "../shader/VulkanShaderModule.h"
#include "scene/camera/CameraComponent.h"
#include "scene/mesh/MeshComponent.h"
#include "scene/SceneObjectComponent.h"
#include "scene/SceneObjectBase.h"
#include "../DataStructures.h"
#include "scene/Transform.h"
#include "data/DataManager.h"
#include "../PerFrameData.h"
#include "utils/ImageUtils.h"

PassBase::PassBase(HashString inName)
	: name(inName)
{

}

PassBase::~PassBase()
{

}

void PassBase::Create()
{
	renderer = Engine::GetRendererInstance();
	vulkanDevice = &renderer->GetDevice();
	//width = renderer->GetWidth();
	//height = renderer->GetHeight();

	renderPass = CreateRenderPass();
	if (renderPass)
	{
		CreateColorAttachments(attachments, attachmentViews, width, height);
		if (!isDepthExternal)
		{
			CreateDepthAttachment(depthAttachment, depthAttachmentView, width, height);
		}
		std::vector<ImageView> views = attachmentViews;
		if (depthAttachmentView)
		{
			views.push_back(depthAttachmentView);
		}
		framebuffer = CreateFramebuffer(renderPass, views, width, height);
	}

	OnCreate();
}

void PassBase::Destroy()
{
	OnDestroy();

	device& device = vulkanDevice->GetDevice();

	PipelineRegistry::GetInstance()->DestroyPipelines(vulkanDevice, name);

	device.destroyRenderPass(renderPass);
	device.destroyFramebuffer(framebuffer);
	for (uint32_t index = 0; index < attachments.size(); index++)
	{
		device.destroyImageView(attachmentViews[index]);
		attachments[index].Destroy();
	}
	if (!isDepthExternal)
	{
		device.destroyImageView(depthAttachmentView);
		depthAttachment.Destroy();
	}
}

void PassBase::SetResolution(uint32_t inWidth, uint32_t inHeight)
{
	width = inWidth;
	height = inHeight;
}

void PassBase::SetExternalDepth(const VulkanImage& inDepthAttachment, const ImageView& inDepthAttachmentView)
{
	isDepthExternal = inDepthAttachment && inDepthAttachmentView;
	if (isDepthExternal)
	{
		depthAttachment = inDepthAttachment;
		depthAttachmentView = inDepthAttachmentView;
	}
}

Framebuffer PassBase::CreateFramebuffer(RenderPass inRenderPass, std::vector<ImageView>& inAttachmentViews, uint32_t inWidth, uint32_t inHeight)
{
	FramebufferCreateInfo framebufferInfo;
	framebufferInfo.setRenderPass(inRenderPass);
	framebufferInfo.setAttachmentCount(static_cast<uint32_t>( inAttachmentViews.size() ));
	framebufferInfo.setPAttachments(inAttachmentViews.data());
	framebufferInfo.setWidth(inWidth);
	framebufferInfo.setHeight(inHeight);
	framebufferInfo.setLayers(1);

	return vulkanDevice->GetDevice().createFramebuffer(framebufferInfo);
}

PipelineLayout PassBase::CreatePipelineLayout(std::vector<DescriptorSetLayout>& inDescriptorSetLayouts)
{
	device& device = vulkanDevice->GetDevice();

	PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setFlags(PipelineLayoutCreateFlags());
	pipelineLayoutInfo.setSetLayoutCount(static_cast<uint32_t>( inDescriptorSetLayouts.size() ));
	pipelineLayoutInfo.setPSetLayouts(inDescriptorSetLayouts.data());

	PushConstantRange pushConstRange;
	pushConstRange.setOffset(0);
	pushConstRange.setSize(sizeof(uint32_t));
	pushConstRange.setStageFlags(ShaderStageFlagBits::eAllGraphics);

	pipelineLayoutInfo.setPushConstantRangeCount(1);
	pipelineLayoutInfo.setPPushConstantRanges(&pushConstRange);

	return device.createPipelineLayout(pipelineLayoutInfo);
}

PipelineData& PassBase::FindPipeline(MaterialPtr inMaterial)
{
	device& device = vulkanDevice->GetDevice();

	PipelineRegistry& pipelineRegistry = *PipelineRegistry::GetInstance();
	// check pipeline storage and create new pipeline in case it was not created before
	if (!pipelineRegistry.HasPipeline(name, inMaterial->GetShaderHash()))
	{
		PipelineData pipelineData;

		inMaterial->CreateDescriptorSet(vulkanDevice);
		pipelineData.descriptorSets = { renderer->GetPerFrameData()->GetSet(), inMaterial->GetDescriptorSet() };

		std::vector<DescriptorSetLayout> setLayouts = { renderer->GetPerFrameData()->GetLayout(), inMaterial->GetDescriptorSetLayout() };
		pipelineData.pipelineLayout = CreatePipelineLayout(setLayouts);
		pipelineData.pipeline = CreatePipeline(inMaterial, pipelineData.pipelineLayout, GetRenderPass());

		pipelineRegistry.StorePipeline(name, inMaterial->GetShaderHash(), pipelineData);
	}

	return pipelineRegistry.GetPipeline(name, inMaterial->GetShaderHash());
}

DescriptorSetLayout PassBase::CreateDescriptorSetLayout(MaterialPtr inMaterial)
{
	DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.setBindingCount(static_cast<uint32_t>(inMaterial->GetBindings().size()));
	descriptorSetLayoutInfo.setPBindings(inMaterial->GetBindings().data());

	return vulkanDevice->GetDevice().createDescriptorSetLayout(descriptorSetLayoutInfo);
}




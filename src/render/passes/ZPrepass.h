#pragma once

#include "PassBase.h"
#include "vulkan/vulkan.hpp"
#include <array>

using namespace VULKAN_HPP_NAMESPACE;

class ZPrepass : public PassBase
{
public:
	ZPrepass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
	ClearValue clearValue;

	virtual void OnCreate() override;
	virtual void OnDestroy() override {}
	virtual RenderPass CreateRenderPass() override;
	virtual void CreateColorAttachments(
		std::vector<VulkanImage>& outAttachments,
		std::vector<ImageView>& outAttachmentViews,
		uint32_t inWidth,
		uint32_t inHeight) override;
	virtual void CreateDepthAttachment(
		VulkanImage& outDepthAttachment,
		ImageView& outDepthAttachmentView,
		uint32_t inWidth,
		uint32_t inHeight) override;
	virtual Pipeline CreatePipeline(MaterialPtr inMaterial, PipelineLayout inLayout, RenderPass inRenderPass) override;
};

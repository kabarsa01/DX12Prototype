#pragma once

#include "vulkan/vulkan.hpp"
#include "render/passes/PassBase.h"

class PostProcessPass : public PassBase
{
public:
	PostProcessPass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
	MaterialPtr postProcessMaterial;
	Texture2DPtr screenImage;

	virtual void OnCreate() override;
	virtual void OnDestroy() override {}
	RenderPass CreateRenderPass() override;
	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, std::vector<ImageView>& outAttachmentViews, uint32_t inWidth, uint32_t inHeight) override;
	void CreateDepthAttachment(ImageResource& outDepthAttachment, ImageView& outDepthAttachmentView, uint32_t inWidth, uint32_t inHeight) override;
	Pipeline CreatePipeline(MaterialPtr inMaterial, PipelineLayout inLayout, RenderPass inRenderPass) override;
private:
};

//#pragma once
//
//#include "PassBase.h"
//
//class DeferredLightingPass : public PassBase
//{
//public:
//	DeferredLightingPass(HashString inName);
//	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
//protected:
//	MaterialPtr lightingMaterial;
//	Texture2DPtr albedoTexture;
//	Texture2DPtr normalTexture;
//	Texture2DPtr depthTexture;
//
//	virtual void OnCreate() override;
//	virtual void OnDestroy() override {}
//	RenderPass CreateRenderPass() override;
//	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, std::vector<ImageView>& outAttachmentViews, uint32_t inWidth, uint32_t inHeight) override;
//	void CreateDepthAttachment(ImageResource& outDepthAttachment, ImageView& outDepthAttachmentView, uint32_t inWidth, uint32_t inHeight) override;
//	Pipeline CreatePipeline(MaterialPtr inMaterial, PipelineLayout inLayout, RenderPass inRenderPass) override;
//
//};

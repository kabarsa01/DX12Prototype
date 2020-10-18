#pragma once

#include "PassBase.h"

class DeferredLightingPass : public PassBase
{
public:
	DeferredLightingPass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
	MaterialPtr lightingMaterial;
	Texture2DPtr albedoTexture;
	Texture2DPtr normalTexture;
	Texture2DPtr depthTexture;

	virtual void OnCreate() override;
	virtual void OnDestroy() override {}

	void CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews) override;
	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight) override;
	ImageResource CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight) override;
	ResourceView CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock) override;
	ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) override;

};

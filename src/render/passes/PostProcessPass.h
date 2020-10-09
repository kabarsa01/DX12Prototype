#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "render/passes/PassBase.h"

using namespace Microsoft::WRL;

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

	void CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews) override;
	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight) override;
	ImageResource CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight) override;
	ResourceView CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock) override;
	ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) override;
private:
};

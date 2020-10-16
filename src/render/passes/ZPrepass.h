#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "PassBase.h"
#include <array>

using namespace Microsoft;

class ZPrepass : public PassBase
{
public:
	ZPrepass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
	virtual void OnCreate() override;
	virtual void OnDestroy() override {}

	void CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews) override;
	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight) override;
	ImageResource CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight) override;
	ResourceView CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock) override;
	ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) override;

};

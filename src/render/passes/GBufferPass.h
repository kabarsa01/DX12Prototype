#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "PassBase.h"
#include <array>

using namespace Microsoft::WRL;

class GBufferPass : public PassBase
{
public:
	GBufferPass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
//	std::array<ClearValue, 3> clearValues;

	virtual void OnCreate() override;
	virtual void OnDestroy() override {}
//	virtual RenderPass CreateRenderPass() override;
	virtual void CreateColorAttachments(
		std::vector<ImageResource>& outAttachments, 
		uint32_t inWidth, 
		uint32_t inHeight) override;
	virtual void CreateColorAttachmentViews(
		const std::vector<ImageResource>& inAttachments, 
		DescriptorBlock inBlock, 
		std::vector<ResourceView>& outAttachmentViews) override;
	virtual ImageResource CreateDepthAttachment(
		uint32_t inWidth, 
		uint32_t inHeight) override;
	virtual ResourceView CreateDepthAttachmentView(
		const ImageResource& inDepthAttachment, 
		DescriptorBlock inBlock) override;

	virtual ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) override;
};

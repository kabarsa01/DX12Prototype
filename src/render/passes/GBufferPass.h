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
		std::vector<ResourceView>& outAttachmentViews,
		uint32_t inWidth, 
		uint32_t inHeight) override;
	virtual void CreateDepthAttachment(
		ImageResource& outDepthAttachment,
//		ImageView& outDepthAttachmentView,
		uint32_t inWidth,
		uint32_t inHeight) override;
	virtual ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) override;
};

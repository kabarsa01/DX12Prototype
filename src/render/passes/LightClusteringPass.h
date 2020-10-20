#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "PassBase.h"
#include "../resources/ImageResource.h"
#include "../DataStructures.h"

using namespace Microsoft::WRL;

class LightClusteringPass : public PassBase
{
public:
	MaterialPtr computeMaterial;
	Texture2DPtr depthTexture;
	Texture2DPtr debugClustersTexture;

	LightClusteringPass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
	LightClusters* lightClusters;
	LightClustersIndices* lightClustersIndices;
	LightsList* lightsList;
	LightsIndices* lightsIndices;

	virtual void OnCreate() override;
	virtual void OnDestroy() override;

	void CreateColorAttachmentViews(const std::vector<ImageResource>& inAttachments, DescriptorBlock inBlock, std::vector<ResourceView>& outAttachmentViews) override;
	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, uint32_t inWidth, uint32_t inHeight) override;
	ImageResource CreateDepthAttachment(uint32_t inWidth, uint32_t inHeight) override;
	ResourceView CreateDepthAttachmentView(const ImageResource& inDepthAttachment, DescriptorBlock inBlock) override;
	ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) override;

};

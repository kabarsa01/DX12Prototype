#pragma once

#include "vulkan/vulkan.hpp"
#include "PassBase.h"
#include "../resources/ImageResource.h"
#include "../DataStructures.h"

class LightClusteringPass : public PassBase
{

public:
	MaterialPtr computeMaterial;
	ImageResource image;
	ImageView imageView;
	Texture2DPtr texture;
	Texture2DPtr depthTexture;

	LightClusteringPass(HashString inName);
	void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) override;
protected:
	ClusterLightsData* clusterLightData;
	LightsList* lightsList;
	LightsIndices* lightsIndices;

	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	RenderPass CreateRenderPass() override;
	void CreateColorAttachments(std::vector<ImageResource>& outAttachments, std::vector<ImageView>& outAttachmentViews, uint32_t inWidth, uint32_t inHeight) override;
	void CreateDepthAttachment(ImageResource& outDepthAttachment, ImageView& outDepthAttachmentView, uint32_t inWidth, uint32_t inHeight) override;
	Pipeline CreatePipeline(MaterialPtr inMaterial, PipelineLayout inLayout, RenderPass inRenderPass) override;
};

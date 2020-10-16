#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>

#include "../resources/BufferResource.h"
#include "../memory/DeviceMemoryManager.h"
#include "../resources/ImageResource.h"
#include "../PipelineRegistry.h"
#include "data/Material.h"
#include "../resources/ResourceView.h"

class Device;
class Renderer;

using namespace Microsoft::WRL;

class PassBase
{
public:
	PassBase(HashString inName);
	virtual ~PassBase();

	void Create();
	void Destroy();

	void SetResolution(uint32_t inWidth, uint32_t inHeight);

	inline HashString& GetName() { return name; }
	inline const std::vector<ImageResource>& GetAttachments() { return attachments; }
	inline const std::vector<ResourceView>& GetAttachmentViews() { return attachmentViews; }
	inline ImageResource& GetDepthAttachment() { return depthAttachment; }
	inline ResourceView& GetDepthAttachmentView() { return depthAttachmentView; }
	inline DescriptorBlock& GetRtvBlock() { return rtvViews; }
	inline DescriptorBlock& GetDsvBlock() { return dsvViews; }
	inline uint32_t GetWidth() { return width; }
	inline uint32_t GetHeight() { return height; }

	void SetExternalDepth(const ImageResource& inDepthAttachment);

	virtual void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) = 0;
protected:
	inline Device* GetDevice() { return device; }
	inline Renderer* GetRenderer() { return renderer; }

	virtual void OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void CreateColorAttachments(
		std::vector<ImageResource>& outAttachments,
		uint32_t inWidth,
		uint32_t inHeight) = 0;
	virtual void CreateColorAttachmentViews(
		const std::vector<ImageResource>& inAttachments,
		DescriptorBlock inBlock,
		std::vector<ResourceView>& outAttachmentViews) = 0;
	virtual ImageResource CreateDepthAttachment(
		uint32_t inWidth,
		uint32_t inHeight) = 0;
	virtual ResourceView CreateDepthAttachmentView(
		const ImageResource& inDepthAttachment,
		DescriptorBlock inBlock) = 0;
	virtual ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) = 0;

	PipelineData& FindPipeline(MaterialPtr inMaterial);
	ComPtr<ID3D12RootSignature> CreateRootSignature(MaterialPtr inMaterial);
private:
	HashString name;
	Device* device;
	Renderer* renderer;

	std::vector<ImageResource> attachments;
	std::vector<ResourceView> attachmentViews;
	ImageResource depthAttachment;
	ResourceView depthAttachmentView;
	DescriptorBlock rtvViews;
	DescriptorBlock dsvViews;

	uint32_t width = 1280;
	uint32_t height = 720;
	bool isDepthExternal;

	PassBase();
};

#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>

#include "../resources/BufferResource.h"
#include "../memory/DeviceMemoryManager.h"
#include "../resources/ImageResource.h"
#include "../PipelineRegistry.h"
#include "data/Material.h"

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
	inline ImageResource& GetDepthAttachment() { return depthAttachment; }
//	inline ImageView& GetDepthAttachmentView() { return depthAttachmentView; }
	inline uint32_t GetWidth() { return width; }
	inline uint32_t GetHeight() { return height; }

	void SetExternalDepth(const ImageResource& inDepthAttachment/*const ImageView& inDepthAttachmentView*/);

	virtual void RecordCommands(ComPtr<ID3D12GraphicsCommandList> inCommandList) = 0;
protected:
	inline Device* GetDevice() { return device; }
	inline Renderer* GetRenderer() { return renderer; }

	virtual void OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void CreateColorAttachments(
		std::vector<ImageResource>& outAttachments, 
//		std::vector<ImageView>& outAttachmentViews,
		uint32_t inWidth,
		uint32_t inHeight) = 0;
	virtual void CreateDepthAttachment(
		ImageResource& outDepthAttachment,
//		ImageView& outDepthAttachmentView,
		uint32_t inWidth,
		uint32_t inHeight) = 0;
	virtual ComPtr<ID3D12PipelineState> CreatePipeline(MaterialPtr inMaterial, ComPtr<ID3D12RootSignature> inRootSignature) = 0;


	//Framebuffer CreateFramebuffer(
	//	RenderPass inRenderPass,
	//	std::vector<ImageView>& inAttachmentViews,
	//	uint32_t inWidth,
	//	uint32_t inHeight);
	PipelineData& FindPipeline(MaterialPtr inMaterial);
	ComPtr<ID3D12RootSignature> CreateRootSignature();// std::vector<DescriptorSetLayout>& inDescriptorSetLayouts);
//	DescriptorSetLayout CreateDescriptorSetLayout(MaterialPtr inMaterial);
private:
	HashString name;
	Device* device;
	Renderer* renderer;

	//RenderPass renderPass;
	//Framebuffer framebuffer;
	std::vector<ImageResource> attachments;
//	std::vector<ImageView> attachmentViews;
	ImageResource depthAttachment;
//	ImageView depthAttachmentView;
	DescriptorBlock descriptorViews;

	uint32_t width = 1280;
	uint32_t height = 720;
	bool isDepthExternal;

	PassBase() {}
};

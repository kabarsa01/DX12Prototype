#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "../objects/Device.h"
#include "../memory/DeviceMemoryManager.h"
#include "BufferResource.h"
#include <vector>

class ImageResource
{
public:
	D3D12_RESOURCE_DESC resourceDescription;

	ImageResource(bool inScoped = false);
	virtual ~ImageResource();

	void Create(Device* inDevice);
//	ImageView CreateView(ImageSubresourceRange inSubRange, ImageViewType inViewType);
	void Destroy();

	inline uint32_t GetWidth() { return width; }
	inline uint32_t GetHeight() { return height; }
	inline uint32_t GetDepth() { return depth; }
	inline uint32_t GetMips() { return mips; }

//	void BindMemory(MemoryPropertyFlags inMemoryPropertyFlags);

	//void Transfer(CommandBuffer* inCmdBuffer, uint32_t inQueueFamilyIndex);
	//BufferImageCopy CreateBufferImageCopy();
	//void LayoutTransition(CommandBuffer* inCmdBuffer, ImageLayout inOldLayout, ImageLayout inNewLayout);

	//ImageMemoryBarrier CreateBarrier(
	//	ImageLayout inOldLayout,
	//	ImageLayout inNewLayout,
	//	uint32_t inSrcQueue,
	//	uint32_t inDstQueue,
	//	AccessFlags inSrcAccessMask,
	//	AccessFlags inDstAccessMask,
	//	ImageAspectFlags inAspectFlags,
	//	uint32_t inBaseMipLevel,
	//	uint32_t inMipLevelCount,
	//	uint32_t inBaseArrayLayer,
	//	uint32_t inArrayLayerCount
	//) const;
	//ImageMemoryBarrier CreateBarrier(
	//	ImageLayout inOldLayout,
	//	ImageLayout inNewLayout,
	//	uint32_t inSrcQueue,
	//	uint32_t inDstQueue,
	//	AccessFlags inSrcAccessMask,
	//	AccessFlags inDstAccessMask,
	//	ImageSubresourceRange inSubresourceRange
	//) const;
	//ImageMemoryBarrier CreateLayoutBarrier(
	//	ImageLayout inOldLayout,
	//	ImageLayout inNewLayout,
	//	AccessFlags inSrcAccessMask,
	//	AccessFlags inDstAccessMask,
	//	ImageAspectFlags inAspectFlags,
	//	uint32_t inBaseMipLevel,
	//	uint32_t inMipLevelCount,
	//	uint32_t inBaseArrayLayer,
	//	uint32_t inArrayLayerCount
	//) const;
	inline D3D12_RESOURCE_ALLOCATION_INFO GetAllocationInfo() { return allocInfo; }
	inline ComPtr<ID3D12Resource> GetResource() const { return resource; }
	BufferResource* CreateStagingBuffer();
	inline BufferResource* GetStagingBuffer() { return stagingBuffer; }
	void DestroyStagingBuffer();

	operator ComPtr<ID3D12Resource>() const { return resource; }
	operator ID3D12Resource*() const { return resource.Get(); }
	operator bool() const { return resource.Get() != nullptr; }
protected:
	Device* device;
	ComPtr<ID3D12Resource> resource;
	MemoryRecord memoryRecord;
	BufferResource* stagingBuffer;
	D3D12_RESOURCE_ALLOCATION_INFO allocInfo;
//	std::vector<char> data;

	bool scoped;
	uint32_t width = 2;
	uint32_t height = 2;
	uint32_t depth = 1;
	uint32_t mips = 1;
};



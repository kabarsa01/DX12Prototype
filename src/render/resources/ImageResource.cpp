#include "ImageResource.h"
#include "core/Engine.h"
#include "d3dx12.h"

ImageResource::ImageResource(bool inScoped)
	: scoped(inScoped)
	, resource(nullptr)
	, stagingBuffer(nullptr)
{

}

ImageResource::~ImageResource()
{
	if (scoped)
	{
		Destroy();
	}
}

void ImageResource::Create(Device* inDevice, D3D12_RESOURCE_STATES inInitialStates/* = D3D12_RESOURCE_STATE_COPY_DEST*/)
{
	if (resource || !inDevice)
	{
		return;
	}
	device = inDevice;

	allocInfo = device->GetNativeDevice()->GetResourceAllocationInfo(0, 1, &resourceDescription);

	DeviceMemoryManager* dmm = DeviceMemoryManager::GetInstance();
	memoryRecord = dmm->RequestMemory(allocInfo.SizeInBytes, D3D12_HEAP_TYPE_DEFAULT);

	MemoryPosition& memPos = memoryRecord.pos;
	device->GetNativeDevice()->CreatePlacedResource(
		memPos.memory.GetHeap().Get(), 
		memPos.offset, 
		&resourceDescription, 
		inInitialStates,
		nullptr, 
		IID_PPV_ARGS(&resource));

	width = resourceDescription.Width;
	height = resourceDescription.Height;
	depth = resourceDescription.DepthOrArraySize;
	mips = resourceDescription.MipLevels;
}

//ImageView VulkanImage::CreateView(ImageSubresourceRange inSubRange, ImageViewType inViewType)
//{
//	ImageViewCreateInfo imageViewInfo;
//	imageViewInfo.setComponents(ComponentMapping());
//	imageViewInfo.setFormat(createInfo.format);
//	imageViewInfo.setImage(resource);
//	imageViewInfo.setSubresourceRange(inSubRange);
//	imageViewInfo.setViewType(inViewType);
//
//	return device->GetDevice().createImageView(imageViewInfo);
//}

void ImageResource::Destroy()
{
	DestroyStagingBuffer();
	if (resource.Get() != nullptr)
	{
		resource = nullptr;
		DeviceMemoryManager::GetInstance()->ReturnMemory(memoryRecord);
	}
}

//void VulkanImage::Transfer(CommandBuffer* inCmdBuffer, uint32_t inQueueFamilyIndex)
//{
//	if (!stagingBuffer)
//	{
//		CreateStagingBuffer(SharingMode::eExclusive, inQueueFamilyIndex);
//	}
//
//	std::array<BufferImageCopy, 1> copyArray = { CreateBufferImageCopy() };
//	inCmdBuffer->copyBufferToImage(stagingBuffer, resource, ImageLayout::eTransferDstOptimal, 1, copyArray.data());
//}
//
//BufferImageCopy VulkanImage::CreateBufferImageCopy()
//{
//	BufferImageCopy imageCopy;
//	imageCopy.setBufferOffset(0);
//	imageCopy.setBufferImageHeight(0);
//	imageCopy.setBufferRowLength(0);
//	imageCopy.setImageExtent(Extent3D(width, height, depth));
//	imageCopy.setImageOffset(Offset3D(0, 0, 0));
//	imageCopy.imageSubresource.setAspectMask(ImageAspectFlagBits::eColor);
//	imageCopy.imageSubresource.setBaseArrayLayer(0);
//	imageCopy.imageSubresource.setLayerCount(1);
//	imageCopy.imageSubresource.setMipLevel(0);
//
//	return imageCopy;
//}
//
//void VulkanImage::LayoutTransition(CommandBuffer* inCmdBuffer, ImageLayout inOldLayout, ImageLayout inNewLayout)
//{
//	ImageMemoryBarrier barrier = CreateLayoutBarrier(
//		inOldLayout,
//		inNewLayout,
//		AccessFlagBits::eColorAttachmentWrite,
//		AccessFlagBits::eShaderRead,
//		ImageAspectFlagBits::eColor,
//		0, 1, 0, 1
//	);
//
//	inCmdBuffer->pipelineBarrier(PipelineStageFlagBits::eColorAttachmentOutput, PipelineStageFlagBits::eFragmentShader, DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
//}
//
//ImageMemoryBarrier VulkanImage::CreateBarrier(
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
//	uint32_t inArrayLayerCount) const
//{
//	ImageMemoryBarrier barrier;
//	barrier.setImage(resource);
//	barrier.setOldLayout(inOldLayout);
//	barrier.setNewLayout(inNewLayout);
//	barrier.setSrcQueueFamilyIndex(inSrcQueue);
//	barrier.setDstQueueFamilyIndex(inDstQueue);
//	barrier.subresourceRange.setAspectMask(inAspectFlags);
//	barrier.subresourceRange.setBaseMipLevel(inBaseMipLevel);
//	barrier.subresourceRange.setLevelCount(inMipLevelCount);
//	barrier.subresourceRange.setBaseArrayLayer(inBaseArrayLayer);
//	barrier.subresourceRange.setLayerCount(inArrayLayerCount);
//	barrier.setSrcAccessMask(inSrcAccessMask);
//	barrier.setDstAccessMask(inDstAccessMask);
//
//	return barrier;
//}
//
//ImageMemoryBarrier VulkanImage::CreateBarrier(
//	ImageLayout inOldLayout, 
//	ImageLayout inNewLayout, 
//	uint32_t inSrcQueue, 
//	uint32_t inDstQueue, 
//	AccessFlags inSrcAccessMask, 
//	AccessFlags inDstAccessMask, 
//	ImageSubresourceRange inSubresourceRange) const
//{
//	ImageMemoryBarrier barrier;
//	barrier.setImage(resource);
//	barrier.setOldLayout(inOldLayout);
//	barrier.setNewLayout(inNewLayout);
//	barrier.setSrcQueueFamilyIndex(inSrcQueue);
//	barrier.setDstQueueFamilyIndex(inDstQueue);
//	barrier.setSubresourceRange(inSubresourceRange);
//	barrier.setSrcAccessMask(inSrcAccessMask);
//	barrier.setDstAccessMask(inDstAccessMask);
//
//	return barrier;
//}
//
//ImageMemoryBarrier VulkanImage::CreateLayoutBarrier(
//	ImageLayout inOldLayout, 
//	ImageLayout inNewLayout, 
//	AccessFlags inSrcAccessMask, 
//	AccessFlags inDstAccessMask, 
//	ImageAspectFlags inAspectFlags, 
//	uint32_t inBaseMipLevel, 
//	uint32_t inMipLevelCount, 
//	uint32_t inBaseArrayLayer, 
//	uint32_t inArrayLayerCount) const
//{
//	return CreateBarrier(
//		inOldLayout,
//		inNewLayout,
//		VK_QUEUE_FAMILY_IGNORED,
//		VK_QUEUE_FAMILY_IGNORED,
//		inSrcAccessMask,
//		inDstAccessMask,
//		inAspectFlags,
//		inBaseMipLevel,
//		inMipLevelCount,
//		inBaseArrayLayer,
//		inArrayLayerCount
//	);
//}

BufferResource* ImageResource::CreateStagingBuffer()
{
	if (stagingBuffer)
	{
		return stagingBuffer;
	}
	stagingBuffer = new BufferResource();
	//GetRequiredIntermediateSize
	stagingBuffer->Create(device, allocInfo.SizeInBytes, D3D12_HEAP_TYPE_UPLOAD);
	return stagingBuffer;
}

void ImageResource::DestroyStagingBuffer()
{
	if (stagingBuffer)
	{
		stagingBuffer->Destroy();
		delete stagingBuffer;
	}
}


#include "BufferResource.h"
#include "core/Engine.h"
#include "../TransferList.h"
#include "utils/HelperUtils.h"

BufferResource::BufferResource(bool inScoped, bool inCleanup)
	: scoped(inScoped)
	, cleanup(inCleanup)
	, stagingBuffer(nullptr)
{

}

BufferResource::~BufferResource()
{
	if (scoped)
	{
		Destroy();
	}
}

void BufferResource::Create(Device* inDevice, uint64_t inSize, D3D12_HEAP_TYPE inHeapType = D3D12_HEAP_TYPE_DEFAULT)
{
	if (resource.Get())
	{
		return;
	}
	device = inDevice;

	D3D12_RESOURCE_DESC desc;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.DepthOrArraySize = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = inSize;

	D3D12_RESOURCE_ALLOCATION_INFO allocInfo = device->GetNativeDevice()->GetResourceAllocationInfo(0, 1, &desc);
	DeviceMemoryManager* dmm = DeviceMemoryManager::GetInstance();
	memRecord = dmm->RequestMemory(allocInfo.SizeInBytes, inHeapType);

	D3D12_CLEAR_VALUE clearValue;
	ThrowIfFailed( device->GetNativeDevice()->CreatePlacedResource(
		memRecord.pos.memory.GetHeap().Get(), 
		memRecord.pos.offset,
		&desc, 
		D3D12_RESOURCE_STATE_COMMON, 
		&clearValue, 
		IID_PPV_ARGS(&resource)) );

	size = allocInfo.SizeInBytes;
	heapType = inHeapType;
}

void BufferResource::SetData(const std::vector<char>& inData)
{
	data = inData;
}

void BufferResource::SetData(uint64_t inSize, const char* inData)
{
	data.assign(inData, inData + inSize);
}

void BufferResource::CopyTo(uint64_t inSize, const char* inData, bool pushToTransfer)
{
	if (stagingBuffer)
	{
		CopyToStagingBuffer(inSize, inData);
		if (pushToTransfer)
		{
			TransferList::GetInstance()->PushBuffer(this);
		}
	}
	else
	{
		CopyToBuffer(inSize, inData);
	}
}

void BufferResource::CopyToBuffer(uint64_t inSize, const char* inData)
{
	void* mappedMem;
	ThrowIfFailed( resource->Map(0, nullptr, &mappedMem) );
	memcpy(mappedMem, inData, inSize);
	resource->Unmap(0, nullptr);
}

void BufferResource::CopyToStagingBuffer(uint64_t inSize, const char* inData)
{
	if (!stagingBuffer)
	{
		CreateStagingBuffer();
	}
	stagingBuffer->CopyToBuffer(inSize, inData);
}

void BufferResource::Destroy()
{
	if (!cleanup)
	{
		return;
	}
	if (stagingBuffer != nullptr)
	{
		stagingBuffer->Destroy();
		delete stagingBuffer;
		stagingBuffer = nullptr;
	}
	if (resource)
	{
		resource = nullptr;
		DeviceMemoryManager::GetInstance()->ReturnMemory(memRecord);
	}
}

BufferResource* BufferResource::CreateStagingBuffer()
{
	if (stagingBuffer)
	{
		return stagingBuffer;
	}
	stagingBuffer = new BufferResource();
	stagingBuffer->Create(device, size, D3D12_HEAP_TYPE_UPLOAD);
	return stagingBuffer;
}

//BufferCopy VulkanBuffer::CreateBufferCopy()
//{
//	BufferCopy copyRegion;
//	copyRegion.setSize(createInfo.size);
//	copyRegion.setSrcOffset(0);
//	copyRegion.setDstOffset(0);
//
//	return copyRegion;
//}
//
//BufferMemoryBarrier VulkanBuffer::CreateMemoryBarrier(uint32_t inSrcQueue, uint32_t inDstQueue, AccessFlags inSrcAccessMask, AccessFlags inDstAccessMask)
//{
//	BufferMemoryBarrier barrier;
//
//	barrier.setBuffer(resource);
//	barrier.setSize(createInfo.size);
//	barrier.setOffset(0);
//	barrier.setSrcQueueFamilyIndex(inSrcQueue);
//	barrier.setDstQueueFamilyIndex(inDstQueue);
//	barrier.setSrcAccessMask(inSrcAccessMask);
//	barrier.setDstAccessMask(inDstAccessMask);
//
//	return barrier;
//}
//
//DescriptorBufferInfo& VulkanBuffer::GetDescriptorInfo()
//{
//	return descriptorInfo;
//}





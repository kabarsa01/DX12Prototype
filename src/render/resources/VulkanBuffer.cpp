#include "VulkanBuffer.h"
#include "core/Engine.h"
#include "../TransferList.h"

VulkanBuffer::VulkanBuffer(bool inScoped, bool inCleanup)
	: scoped(inScoped)
	, cleanup(inCleanup)
	, stagingBuffer(nullptr)
{

}

VulkanBuffer::~VulkanBuffer()
{
	if (scoped)
	{
		Destroy();
	}
}

void VulkanBuffer::Create(Device* inDevice)
{
	if (buffer)
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

	buffer = device->GetNativeDevice()->CreatePlacedResource(nullptr, 0, );

	descriptorInfo.setBuffer(buffer);
	descriptorInfo.setOffset(0);
	descriptorInfo.setRange(createInfo.size);
}

void VulkanBuffer::SetData(const std::vector<char>& inData)
{
	data = inData;
}

void VulkanBuffer::SetData(DeviceSize inSize, const char* inData)
{
	data.assign(inData, inData + inSize);
}

void VulkanBuffer::CopyTo(DeviceSize inSize, const char* inData, bool pushToTransfer)
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

void VulkanBuffer::CopyToBuffer(DeviceSize inSize, const char* inData)
{
	memRecord.pos.memory.MapCopyUnmap(MemoryMapFlags(), memRecord.pos.offset, inSize, inData, 0, inSize);
}

void VulkanBuffer::CopyToStagingBuffer(DeviceSize inSize, const char* inData)
{
	MemoryRecord& memRec = stagingBuffer->GetMemoryRecord();
	memRec.pos.memory.MapCopyUnmap(MemoryMapFlags(), memRec.pos.offset, inSize, inData, 0, inSize);
}

void VulkanBuffer::Destroy()
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
	if (buffer)
	{
		device->GetDevice().destroyBuffer(buffer);
		buffer = nullptr;
		DeviceMemoryManager::GetInstance()->ReturnMemory(memRecord);
	}
}

void VulkanBuffer::BindMemory(MemoryPropertyFlags inMemPropertyFlags)
{
	if (memRecord.pos.valid)
	{
		return;
	}
	DeviceMemoryManager* dmm = DeviceMemoryManager::GetInstance();
	memRecord = dmm->RequestMemory(GetMemoryRequirements(), inMemPropertyFlags);
	device->GetDevice().bindBufferMemory(buffer, memRecord.pos.memory, memRecord.pos.offset);
}

void VulkanBuffer::BindMemory(const DeviceMemory& inDeviceMemory, DeviceSize inMemOffset)
{
	device->GetDevice().bindBufferMemory(buffer, inDeviceMemory, inMemOffset);
}

VulkanBuffer* VulkanBuffer::CreateStagingBuffer()
{
	return CreateStagingBuffer(createInfo.size, data.data());
}

VulkanBuffer* VulkanBuffer::CreateStagingBuffer(DeviceSize inSize, char* inData)
{
	if (stagingBuffer)
	{
		return stagingBuffer;
	}

	stagingBuffer = new VulkanBuffer();
	stagingBuffer->createInfo.setSize(inSize);
	stagingBuffer->createInfo.setUsage(BufferUsageFlagBits::eTransferSrc);
	stagingBuffer->createInfo.setSharingMode(SharingMode::eExclusive);
	stagingBuffer->Create(device);
	stagingBuffer->BindMemory(MemoryPropertyFlagBits::eHostCoherent | MemoryPropertyFlagBits::eHostVisible);
	if (inData)
	{
		MemoryRecord& memRec = stagingBuffer->GetMemoryRecord();
		memRec.pos.memory.MapCopyUnmap(MemoryMapFlags(), memRec.pos.offset, inSize, inData, 0, inSize);
	}

	return stagingBuffer;
}

BufferCopy VulkanBuffer::CreateBufferCopy()
{
	BufferCopy copyRegion;
	copyRegion.setSize(createInfo.size);
	copyRegion.setSrcOffset(0);
	copyRegion.setDstOffset(0);

	return copyRegion;
}

BufferMemoryBarrier VulkanBuffer::CreateMemoryBarrier(uint32_t inSrcQueue, uint32_t inDstQueue, AccessFlags inSrcAccessMask, AccessFlags inDstAccessMask)
{
	BufferMemoryBarrier barrier;

	barrier.setBuffer(buffer);
	barrier.setSize(createInfo.size);
	barrier.setOffset(0);
	barrier.setSrcQueueFamilyIndex(inSrcQueue);
	barrier.setDstQueueFamilyIndex(inDstQueue);
	barrier.setSrcAccessMask(inSrcAccessMask);
	barrier.setDstAccessMask(inDstAccessMask);

	return barrier;
}

DescriptorBufferInfo& VulkanBuffer::GetDescriptorInfo()
{
	return descriptorInfo;
}

Buffer& VulkanBuffer::GetBuffer()
{
	return buffer;
}

Buffer VulkanBuffer::GetBuffer() const
{
	return buffer;
}

MemoryRequirements VulkanBuffer::GetMemoryRequirements()
{
	return device->GetDevice().getBufferMemoryRequirements(buffer);
}

MemoryRecord& VulkanBuffer::GetMemoryRecord()
{
	return memRecord;
}



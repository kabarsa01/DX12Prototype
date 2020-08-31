#pragma once

#include "vulkan/vulkan.hpp"
#include "../memory/DeviceMemoryManager.h"
#include "../objects/Device.h"
#include <vector>

using namespace VULKAN_HPP_NAMESPACE;

class VulkanBuffer
{
public:
	BufferCreateInfo createInfo;

	VulkanBuffer(bool inScoped = false, bool inCleanup = true);
	virtual ~VulkanBuffer();

	void Create(VulkanDevice* inDevice);
	void Destroy();

	void SetData(const std::vector<char>& inData);
	void SetData(DeviceSize inSize, const char* inData);
	void CopyTo(DeviceSize inSize, const char* inData, bool pushToTransfer = false);
	void CopyToBuffer(DeviceSize inSize, const char* inData);
	void CopyToStagingBuffer(DeviceSize inSize, const char* inData);
	void BindMemory(MemoryPropertyFlags inMemPropertyFlags);
	void BindMemory(const DeviceMemory& inDeviceMemory, DeviceSize inMemOffset);

	VulkanBuffer* CreateStagingBuffer();
	VulkanBuffer* CreateStagingBuffer(DeviceSize inSize, char* inData);
	BufferCopy CreateBufferCopy();
	BufferMemoryBarrier CreateMemoryBarrier(uint32_t inSrcQueue, uint32_t inDstQueue, AccessFlags inSrcAccessMask, AccessFlags inDstAccessMask);
	DescriptorBufferInfo& GetDescriptorInfo();

	Buffer& GetBuffer();
	Buffer GetBuffer() const;
	MemoryRequirements GetMemoryRequirements();
	MemoryRecord& GetMemoryRecord();

	operator Buffer() const { return buffer; }
	operator bool() const { return buffer; }

	void SetCleanup(bool inCleanup) { cleanup = inCleanup; }

	//static void SubmitCopyCommand(const VulkanBuffer& inSrc, const VulkanBuffer& inDst);
protected:
	VulkanDevice* vulkanDevice;
	Buffer buffer;
	DescriptorBufferInfo descriptorInfo;
	MemoryRecord memRecord;
	std::vector<char> data;

	bool scoped = false;
	bool cleanup = true;

	VulkanBuffer* stagingBuffer;
};
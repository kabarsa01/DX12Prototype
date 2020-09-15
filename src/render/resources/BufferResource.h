#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "../memory/DeviceMemoryManager.h"
#include "../objects/Device.h"
#include <vector>

using namespace Microsoft::WRL;

class BufferResource
{
public:
	BufferResource(bool inScoped = false, bool inCleanup = true);
	virtual ~BufferResource();

	void Create(Device* inDevice, uint64_t inSize, D3D12_HEAP_TYPE inHeapType = D3D12_HEAP_TYPE_DEFAULT);
	void Destroy();

	void SetData(const std::vector<char>& inData);
	void SetData(uint64_t inSize, const char* inData);
	void CopyTo(uint64_t inSize, const char* inData, bool pushToTransfer = false);
	void CopyToBuffer(uint64_t inSize, const char* inData);
	void CopyToStagingBuffer(uint64_t inSize, const char* inData);

	BufferResource* CreateStagingBuffer();
//	VulkanBuffer* CreateStagingBuffer(uint64_t inSize, char* inData);
//	BufferCopy CreateBufferCopy();
//	BufferMemoryBarrier CreateMemoryBarrier(uint32_t inSrcQueue, uint32_t inDstQueue, AccessFlags inSrcAccessMask, AccessFlags inDstAccessMask);
//	DescriptorBufferInfo& GetDescriptorInfo();

	ComPtr<ID3D12Resource> GetResource() const;
	MemoryRecord& GetMemoryRecord();

	operator ComPtr<ID3D12Resource>() const { return resource; }
	operator bool() const { return resource; }

	void SetCleanup(bool inCleanup) { cleanup = inCleanup; }
protected:
	Device* device;
	ComPtr<ID3D12Resource> resource;
	MemoryRecord memRecord;
	std::vector<char> data;
	uint64_t size;
	D3D12_HEAP_TYPE heapType;

	bool scoped = false;
	bool cleanup = true;

	BufferResource* stagingBuffer;
};
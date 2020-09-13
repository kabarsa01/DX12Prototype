#pragma once

#include <d3d12.h>
#include <deque>
#include "../resources/DeviceMemory.h"

struct MemoryPosition
{
	bool valid = false;
	uint32_t layer;
	uint32_t index;
	uint64_t offset;
	DeviceMemory memory;

	MemoryPosition()
		: valid(false)
		, layer(-1)
		, index(-1)
		, offset(-1)
		, memory()
	{}
};

class Device;

class DeviceMemoryChunk
{
public:
	DeviceMemoryChunk(uint64_t inSegmentSize, uint32_t inTreeDepth);
	DeviceMemoryChunk(const DeviceMemoryChunk& inOther);
	virtual ~DeviceMemoryChunk();

	void Allocate(Device* inDevice, D3D12_HEAP_TYPE inHeapType);

	MemoryPosition AcquireSegment(uint64_t inSize);
	void ReleaseSegment(const MemoryPosition& inMemoryPosition);

	DeviceMemory& GetMemory();
	bool HasFreeSpace();
protected:
	uint32_t treeDepth;
	uint32_t treeSize;
	uint32_t segmentCount;
	// flattened binary tree for memory segment tracking
	unsigned char* memoryTree;
	DeviceMemory memory;
	uint64_t segmentSize;

	uint32_t GetLayerStartIndex(uint32_t inLayer);
	uint32_t GetParentIndex(uint32_t inIndex);
	uint32_t GetChildIndex(uint32_t inIndex);
	uint32_t GetSiblingIndex(uint32_t inIndex);
	uint64_t CalculateOffset(uint32_t inLayer, uint32_t inIndex);

	bool FindSegmentIndex(uint32_t inStartLayer, uint32_t inTargetLayer, uint32_t inIndex, uint32_t& outTargetIndex);
	void MarkFreeUp(uint32_t inStartLayer, uint32_t inIndex);
	void MarkNotFreeUp(uint32_t inStartLayer, uint32_t inIndex);
};

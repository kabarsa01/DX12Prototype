#pragma once

#include <chrono>
#include <map>
#include <vector>
#include "DeviceMemoryChunk.h"

struct MemoryRecord
{
	uint64_t regionHash;
	uint64_t chunkIndex;
	MemoryPosition pos;
};

// 2 to 12th is 4096, so we will have a tree with 4096 segments at layer 0, it's tree's leaf nodes
// it's a binary tree with layer 0 being the leaf nodes, and leaf nodes being the smallest memory
// segments available to occupy
static const uint32_t memoryTreeDepth = 13;
// base memory segment size, the smallest you can allocate
// this make easy managing alignment, min size is exactly default d3d alignment
static const uint64_t baseMemorySegmentSize = 65536;

class DeviceMemoryManager
{
public:
	static DeviceMemoryManager* GetInstance();

	MemoryRecord RequestMemory(uint32_t inSize, D3D12_HEAP_TYPE inHeapType);
	void ReturnMemory(const MemoryRecord& inMemoryRecord);
	void CleanupMemory();

	DeviceMemoryChunk* GetMemoryChunk(MemoryRecord inMemPosition);
protected:
	static DeviceMemoryManager* staticInstance;

	std::map<uint64_t, std::vector<DeviceMemoryChunk*>> memRegions;

	DeviceMemoryManager();
	DeviceMemoryManager(const DeviceMemoryManager&) {}
	void operator= (const DeviceMemoryManager&) {}
	virtual ~DeviceMemoryManager();
};

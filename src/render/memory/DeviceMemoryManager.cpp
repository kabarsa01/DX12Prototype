#include "DeviceMemoryManager.h"
#include <algorithm>
#include "core/Engine.h"
#include "../Renderer.h"

DeviceMemoryManager* DeviceMemoryManager::staticInstance = new DeviceMemoryManager();

DeviceMemoryManager::DeviceMemoryManager()
{

}

DeviceMemoryManager::~DeviceMemoryManager()
{

}

DeviceMemoryManager* DeviceMemoryManager::GetInstance()
{
	return staticInstance;
}

MemoryRecord DeviceMemoryManager::RequestMemory(uint32_t inSize, D3D12_HEAP_TYPE inHeapType)
{
	auto startTime = std::chrono::high_resolution_clock::now();

	MemoryRecord memoryRecord;

	uint64_t regionHash = inHeapType;
	uint64_t requiredSize = inSize;

	std::vector<DeviceMemoryChunk*>& chunkArray = memRegions[regionHash];
	for (uint64_t index = 0; index < chunkArray.size(); index++)
	{
		DeviceMemoryChunk* chunk = chunkArray[index];
		if (chunk->HasFreeSpace())
		{
			MemoryPosition pos = chunk->AcquireSegment(requiredSize);
			if (pos.valid)
			{
				memoryRecord.regionHash = regionHash;
				memoryRecord.chunkIndex = index;
				memoryRecord.pos = pos;

				auto currentTime = std::chrono::high_resolution_clock::now();
				double deltaTime = std::chrono::duration<double, std::chrono::microseconds::period>(currentTime - startTime).count();

				std::printf("suballocation from %I64u bytes memtype %I64u for %I64u took %f microseconds\n", GetRangeBase(static_cast<uint32_t>(rangeIndex)) * 2048, memTypeIndex, requiredSize, deltaTime);

				return memoryRecord;
			}
		}
	}

	chunkArray.push_back(new DeviceMemoryChunk(baseMemorySegmentSize, memoryTreeDepth));

	startTime = std::chrono::high_resolution_clock::now();

	DeviceMemoryChunk* chunk = chunkArray.back();
	chunk->Allocate(&Engine::GetRendererInstance()->GetDevice(), inHeapType);

	auto currentTime = std::chrono::high_resolution_clock::now();
	double deltaTime = std::chrono::duration<double, std::chrono::microseconds::period>(currentTime - startTime).count();

	std::printf("vulkan allocation of %I64u bytes memtype %I64u for %I64u took %f microseconds\n", GetRangeBase(static_cast<uint32_t>(rangeIndex)) * 2048, memTypeIndex, requiredSize, deltaTime);

	MemoryPosition pos = chunk->AcquireSegment(requiredSize);

	memoryRecord.regionHash = regionHash;
	memoryRecord.chunkIndex = chunkArray.size() - 1;
	memoryRecord.pos = pos;

	currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::chrono::microseconds::period>(currentTime - startTime).count();

	return memoryRecord;
}

void DeviceMemoryManager::ReturnMemory(const MemoryRecord& inMemoryRecord)
{
	memRegions[inMemoryRecord.regionHash][inMemoryRecord.chunkIndex]->ReleaseSegment(inMemoryRecord.pos);
}

void DeviceMemoryManager::CleanupMemory()
{
	std::map<D3D12_HEAP_TYPE, std::vector<DeviceMemoryChunk*>>::iterator regionIter;
	for (regionIter = memRegions.begin(); regionIter != memRegions.end(); regionIter++)
	{
		std::vector<DeviceMemoryChunk*>& chunks = regionIter->second;
		for (uint64_t index = 0; index < chunks.size(); index++)
		{
			delete chunks[index];
		}
	}
}

DeviceMemoryChunk* DeviceMemoryManager::GetMemoryChunk(MemoryRecord inMemPosition)
{
	return memRegions[inMemPosition.regionHash][inMemPosition.chunkIndex];
}





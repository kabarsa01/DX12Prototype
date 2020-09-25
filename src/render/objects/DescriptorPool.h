#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <list>
#include <vector>
#include "Device.h"
#include "d3dx12.h"

using namespace Microsoft::WRL;

struct DescriptorBlock
{
	DescriptorPool* parent;
	ComPtr<ID3D12DescriptorHeap> heap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	uint16_t head;
	uint16_t size;
	uint32_t descriptorIncrementSize;

	DescriptorBlock() : head(0), size(0) {}

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint16_t inIndex)
	{
		// copy handle
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = cpuHandle;
		// return after offset
		return handle.Offset(inIndex, descriptorIncrementSize);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint16_t inIndex)
	{
		// copy handle
		CD3DX12_GPU_DESCRIPTOR_HANDLE handle = gpuHandle;
		// return after offset
		return handle.Offset(inIndex, descriptorIncrementSize);
	}
};

// simplified descriptor block version without additional data for heap tracking. it should minimize the space required
// to store "free list" data. in this case "free list" is a vector, so making the data stored in it smaller will minimize
// data movement during insertion or removal. since DescriptorPool is designed to be approx 2^11 or 2^12 descriptors in
// size with many pools being managed at higher level, using vector with a compact data structures will probably
// be fast enough and will have the benefit of contiguous memory chunk. binary search will give relatively fast log2(N) 
// search time, so keeping memory blocks order, doing insertion and after that merging free blocks should be relatively
// lightweight. on a small dataset linked list will probably be just as fast, but i like this solution more.
struct DescriptorBlockRecord
{
	uint16_t head;
	uint16_t size;
};
inline bool operator<(const DescriptorBlockRecord& blockRec1, const DescriptorBlockRecord& blockRec2)
{
	return blockRec1.head < blockRec2.head;
}

class DescriptorPool
{
public:
	DescriptorPool();
	virtual ~DescriptorPool();

	void Create(Device* inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inType, uint16_t inDescriptorsCount, bool inShaderVisible);
	void Destroy();

	inline D3D12_DESCRIPTOR_HEAP_TYPE GetType() { return type; }

	DescriptorBlock Allocate(uint16_t inSize);
	void Release(const DescriptorBlock& inBlock);
private:
	Device* device;
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	uint16_t descriptorCount;
	uint32_t descriptorIncrementSize;

	std::vector<DescriptorBlockRecord> freeBlocks;

	void MergeBlocks(uint16_t inFirst, uint16_t inSecond);
};


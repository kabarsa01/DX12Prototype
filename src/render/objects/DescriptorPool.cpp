#include "DescriptorPool.h"

#include <algorithm>
#include "utils/HelperUtils.h"

DescriptorPool::DescriptorPool()
{

}

DescriptorPool::~DescriptorPool()
{

}

void DescriptorPool::Create(Device* inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inType, ComPtr<ID3D12DescriptorHeap> inHeap, uint32_t inHeapOffset, uint16_t inDescriptorsCount)
{
	device = inDevice;
	heap = inHeap;
	heapOffset = inHeapOffset;
	type = inType;
	descriptorCount = inDescriptorsCount;
	descriptorIncrementSize = device->GetNativeDevice()->GetDescriptorHandleIncrementSize(inType);

	DescriptorBlockRecord rec{ 0, descriptorCount };
	freeBlocks.push_back(rec);
}

void DescriptorPool::Destroy()
{
}

DescriptorBlock DescriptorPool::Allocate(uint16_t inSize)
{
	DescriptorBlock block;
	if (freeBlocks.empty() || inSize == 0)
	{
		return block;
	}

	uint16_t blockIndex = 0xffff;
	for (uint16_t index = 0; index < freeBlocks.size(); index++)
	{
		DescriptorBlockRecord& rec = freeBlocks[index];
		if (inSize <= rec.size)
		{
			block.head = rec.head;
			block.size = inSize;

			rec.head += inSize;
			rec.size -= inSize;

			if (rec.size == 0)
			{
				blockIndex = index;
			}

			break;
		}
	}
	if (blockIndex != 0xffff)
	{
		freeBlocks.erase(freeBlocks.begin() + blockIndex);
	}

	if (block.size > 0)
	{
		block.parent = this;
		block.heap = heap;
		block.cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart()).Offset(block.head + heapOffset, descriptorIncrementSize);
		block.gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart()).Offset(block.head + heapOffset, descriptorIncrementSize);
		block.descriptorIncrementSize = descriptorIncrementSize;
	}

	return block;
}

void DescriptorPool::Release(const DescriptorBlock& inBlock)
{
	DescriptorBlockRecord rec{ inBlock.head, inBlock.size };
	if (freeBlocks.empty())
	{
		freeBlocks.push_back(rec);
		return;
	}

	// binary search the insertion point and keep the vector sorted in terms of block head addresses.
	// vector's random access will allow this search be close to log2(N). insertion should be fine
	// for a relatively compact dataset.
	uint16_t index = 0;
	std::vector<DescriptorBlockRecord>::iterator itemIter = std::lower_bound(freeBlocks.begin(), freeBlocks.end(), rec);
	if (itemIter == freeBlocks.end())
	{
		index = static_cast<uint16_t>( freeBlocks.size() );
		freeBlocks.push_back(rec);
	}
	else
	{
		index = static_cast<uint16_t>( std::distance(freeBlocks.begin(), itemIter) );
		freeBlocks.insert(itemIter, rec);
	}

	// merging order is from the end so that the index could stay relevant
	if (index < (freeBlocks.size() - 1))
	{
		MergeBlocks(index, index + 1);
	}
	if (index > 0)
	{
		MergeBlocks(index - 1, index);
	}
}

void DescriptorPool::MergeBlocks(uint16_t inFirst, uint16_t inSecond)
{
	DescriptorBlockRecord rec1 = freeBlocks[inFirst];
	DescriptorBlockRecord rec2 = freeBlocks[inSecond];
	if ( ( rec1.head + rec1.size ) == rec2.head )
	{
		rec1.size += rec2.size;
	}
	freeBlocks.erase(freeBlocks.begin() + inSecond);
}


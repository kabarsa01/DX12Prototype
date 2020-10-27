#include "DescriptorHeaps.h"
#include "Device.h"
#include "../resources/ResourceView.h"
#include "utils/HelperUtils.h"

namespace {
	const uint32_t SHADER_RESOURCES_HEAP_SIZE = 1000 * 1000;
	const uint16_t RTV_HEAP_SIZE = 8192;
	const uint16_t DSV_HEAP_SIZE = 8192;

	const uint16_t SHADER_RESOURCES_POOL_SIZE = 4096;
	const uint16_t RTV_POOL_SIZE = 512;
	const uint16_t DSV_POOL_SIZE = 512;
};

DescriptorHeaps::DescriptorHeaps()
{

}

DescriptorHeaps::~DescriptorHeaps()
{

}

void DescriptorHeaps::Create(Device* inDevice)
{
	device = inDevice;

	heapsByTypeMap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SHADER_RESOURCES_HEAP_SIZE, true);
	heapsByTypeMap[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_HEAP_SIZE, false);
	heapsByTypeMap[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_HEAP_SIZE, false);

	CBV_SRV_UAVPools.push_back(new DescriptorPool());
	RTVPools.push_back(new DescriptorPool());
	DSVPools.push_back(new DescriptorPool());

	CBV_SRV_UAVPools[0]->Create(inDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapsByTypeMap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 0, SHADER_RESOURCES_POOL_SIZE);
	RTVPools[0]->Create(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, heapsByTypeMap[D3D12_DESCRIPTOR_HEAP_TYPE_RTV], 0, RTV_POOL_SIZE);
	DSVPools[0]->Create(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, heapsByTypeMap[D3D12_DESCRIPTOR_HEAP_TYPE_DSV], 0, DSV_POOL_SIZE);
}

void DescriptorHeaps::Destroy()
{
	DeletePools(CBV_SRV_UAVPools);
	DeletePools(RTVPools);
	DeletePools(DSVPools);
}

DescriptorBlock DescriptorHeaps::AllocateDescriptorsCBV_SRV_UAV(uint16_t inBlockSize)
{
	return AllocateDescriptors(inBlockSize, CBV_SRV_UAVPools, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SHADER_RESOURCES_POOL_SIZE);
}

DescriptorBlock DescriptorHeaps::AllocateDescriptorsRTV(uint16_t inBlockSize)
{
	return AllocateDescriptors(inBlockSize, RTVPools, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_POOL_SIZE);
}

DescriptorBlock DescriptorHeaps::AllocateDescriptorsDSV(uint16_t inBlockSize)
{
	return AllocateDescriptors(inBlockSize, DSVPools, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_POOL_SIZE);
}

ComPtr<ID3D12DescriptorHeap> DescriptorHeaps::CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE inType, uint32_t inHeapSize, bool inShaderVisible)
{
	ComPtr<ID3D12DescriptorHeap> heap;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc.Type = inType;
	heapDesc.NodeMask = 0;
	heapDesc.Flags = inShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NumDescriptors = inHeapSize;
	ThrowIfFailed(device->GetNativeDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));

	return heap;
}

DescriptorBlock DescriptorHeaps::AllocateDescriptors(uint16_t inBlockSize, std::vector<DescriptorPool*>& inPools, D3D12_DESCRIPTOR_HEAP_TYPE inType, uint16_t inPoolSize)
{
	uint32_t poolIndex = 0;
	for (; poolIndex < inPools.size(); poolIndex++)
	{
		DescriptorPool* pool = inPools[poolIndex];
		DescriptorBlock block = pool->Allocate(inBlockSize);
		if (block.size > 0)
		{
			return block;
		}
	}

	inPools.push_back(new DescriptorPool());
	inPools.back()->Create(device, inType, heapsByTypeMap[inType], poolIndex * inPoolSize, inPoolSize);

	return inPools.back()->Allocate(inBlockSize);
}

void DescriptorHeaps::DeletePools(std::vector<DescriptorPool*>& inPools)
{
	for (DescriptorPool* pool : inPools)
	{
		pool->Destroy();
		delete pool;
	}
	inPools.clear();
}

void DescriptorHeaps::ReleaseDescriptors(const DescriptorBlock& inBlock)
{
	if (inBlock.parent && inBlock.heap)
	{
		inBlock.parent->Release(inBlock);
	}
}







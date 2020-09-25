#include "DescriptorHeaps.h"
#include "Device.h"
#include "../resources/ResourceView.h"

namespace {
	const uint16_t SHADER_RESOURCES_HEAP_SIZE = 4096;
	const uint16_t RTV_HEAP_SIZE = 512;
	const uint16_t DSV_HEAP_SIZE = 64;
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

	CBV_SRV_UAVPools.push_back(DescriptorPool());
	RTVPools.push_back(DescriptorPool());
	DSVPools.push_back(DescriptorPool());

	CBV_SRV_UAVPools[0].Create(inDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SHADER_RESOURCES_HEAP_SIZE, true);
	RTVPools[0].Create(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_HEAP_SIZE, true);
	DSVPools[0].Create(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_HEAP_SIZE, true);
}

void DescriptorHeaps::Destroy()
{
	//for (DescriptorPool pool : pools)
	//{
	//	device->GetDevice().destroyDescriptorPool(pool);
	//}
}

DescriptorBlock DescriptorHeaps::AllocateDescriptorsCBV_SRV_UAV(uint16_t inBlockSize)
{
	AllocateDescriptors(inBlockSize, CBV_SRV_UAVPools, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SHADER_RESOURCES_HEAP_SIZE, true);
}

DescriptorBlock DescriptorHeaps::AllocateDescriptorsRTV(uint16_t inBlockSize)
{
	AllocateDescriptors(inBlockSize, RTVPools, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_HEAP_SIZE, true);
}

DescriptorBlock DescriptorHeaps::AllocateDescriptorsDSV(uint16_t inBlockSize)
{
	AllocateDescriptors(inBlockSize, DSVPools, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_HEAP_SIZE, true);
}

void DescriptorHeaps::ReleaseDescriptors(const DescriptorBlock& inBlock)
{

}

DescriptorBlock DescriptorHeaps::AllocateDescriptors(uint16_t inBlockSize, std::vector<DescriptorPool>& inPools, D3D12_DESCRIPTOR_HEAP_TYPE inType, uint16_t inHeapSize, bool inShaderVisible)
{
	for (uint32_t poolIndex = 0; poolIndex < inPools.size(); poolIndex++)
	{
		DescriptorPool& pool = inPools[poolIndex];
		DescriptorBlock block = pool.Allocate(inBlockSize);
		if (block.size > 0)
		{
			return block;
		}
	}

	inPools.push_back(DescriptorPool());
	inPools.back().Create(device, inType, inHeapSize, inShaderVisible);

	return inPools.back().Allocate(inBlockSize);
}

void DescriptorHeaps::ReleaseDescriptors(const DescriptorBlock& inBlock)
{
	inBlock.parent->Release(inBlock);
}

//
//std::vector<DescriptorSet> VulkanDescriptorPools::AllocateSet(const std::vector<DescriptorSetLayout>& inLayouts, DescriptorPool& outPool)
//{
//	for (uint32_t index = 0; index < pools.size(); index++)
//	{
//		DescriptorSetAllocateInfo descSetAllocInfo;
//		descSetAllocInfo.setDescriptorPool(pools[index]);
//		descSetAllocInfo.setDescriptorSetCount(static_cast<uint32_t>( inLayouts.size() ));
//		descSetAllocInfo.setPSetLayouts( inLayouts.data() );
//
//		std::vector<DescriptorSet> sets;
//		sets.resize(inLayouts.size());
//		Result result = device->GetDevice().allocateDescriptorSets(&descSetAllocInfo, sets.data());
//		if (result == Result::eSuccess)
//		{
//			outPool = pools[index];
//			return sets;
//		}
//		else
//		{
//			if (index < (pools.size() - 1))
//			{
//				continue;
//			}
//			if (result == Result::eErrorOutOfHostMemory || result == Result::eErrorOutOfDeviceMemory)
//			{
//				// we're out of memory on device or host
//				throw std::exception("Cannot create descriptor pool. Out of memory");
//			}
//			
//			pools.push_back(ConstructDescriptorPool());
//			descSetAllocInfo.setDescriptorPool(pools.back());
//			Result result = device->GetDevice().allocateDescriptorSets(&descSetAllocInfo, sets.data());
//			if (result == Result::eSuccess)
//			{
//				outPool = pools.back();
//				return sets;
//			}
//			else
//			{
//				throw std::exception("Failure. Cannot create descriptor pool.");
//			}
//		}
//	}
//}
//





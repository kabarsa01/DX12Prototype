#include "DescriptorHeaps.h"
#include "Device.h"

namespace {
	const uint16_t SHADER_RESOURCES_HEAP_SIZE = 2048;
	const uint16_t RTV_HEAP_SIZE = 64;
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

	shaderResourcesPools.push_back(DescriptorPool());
	RTVPools.push_back(DescriptorPool());
	DSVPools.push_back(DescriptorPool());

	shaderResourcesPools[0].Create(inDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SHADER_RESOURCES_HEAP_SIZE, true);
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





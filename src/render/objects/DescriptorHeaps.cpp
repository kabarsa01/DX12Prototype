#include "DescriptorHeaps.h"
#include "Device.h"

DescriptorHeaps::DescriptorHeaps()
{

}

DescriptorHeaps::~DescriptorHeaps()
{

}

void DescriptorHeaps::Create(Device* inDevice)
{
	device = inDevice;
	shaderResourcesHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2048);
	samplersHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32);
	RTVHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64);
	DSVHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 64);
//	pools.push_back(ConstructDescriptorPool());
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
//DescriptorPool VulkanDescriptorPools::ConstructDescriptorPool()
//{
//	DescriptorPoolSize uniformPoolSize;
//	uniformPoolSize.setDescriptorCount(2048);
//	uniformPoolSize.setType(DescriptorType::eUniformBuffer);
//	DescriptorPoolSize storageBufferPoolSize;
//	storageBufferPoolSize.setDescriptorCount(64);
//	storageBufferPoolSize.setType(DescriptorType::eStorageBuffer);
//	DescriptorPoolSize samplerPoolSize;
//	samplerPoolSize.setDescriptorCount(16);
//	samplerPoolSize.setType(DescriptorType::eSampler);
//	DescriptorPoolSize imagePoolSize;
//	imagePoolSize.setDescriptorCount(2048);
//	imagePoolSize.setType(DescriptorType::eSampledImage);
//
//	DescriptorPoolSize poolSizes[] = { uniformPoolSize, storageBufferPoolSize, samplerPoolSize, imagePoolSize };
//	DescriptorPoolCreateInfo descPoolInfo;
//	descPoolInfo.setFlags(DescriptorPoolCreateFlagBits::eFreeDescriptorSet | DescriptorPoolCreateFlagBits::eUpdateAfterBind);
//	descPoolInfo.setPoolSizeCount(4);
//	descPoolInfo.setPPoolSizes(poolSizes);
//	descPoolInfo.setMaxSets(512);
//
//	return device->GetDevice().createDescriptorPool(descPoolInfo);
//}

ComPtr<ID3D12DescriptorHeap> DescriptorHeaps::CreateDescriptorHeap(ComPtr<ID3D12Device2> inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inType, uint32_t inDescriptorsCount)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc.Type = inType;
	heapDesc.NodeMask = 0;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NumDescriptors = inDescriptorsCount;

	ComPtr<ID3D12DescriptorHeap> heap;
	inDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	return heap;
}


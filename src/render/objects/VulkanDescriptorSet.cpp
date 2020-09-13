#include "VulkanDescriptorSet.h"
#include "core/Engine.h"
#include "Device.h"
#include "../Renderer.h"

VulkanDescriptorSet::VulkanDescriptorSet()
{
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{

}

void VulkanDescriptorSet::Create(Device* inVulkanDevice)
{
	vulkanDevice = inVulkanDevice;

//	set = Engine::GetRendererInstance()->GetDescriptorHeaps().AllocateSet({layout}, pool)[0];
}

void VulkanDescriptorSet::Create(Device* inVulkanDevice, std::vector<VulkanDescriptorSet*>& inSets)
{
	Create(inVulkanDevice, static_cast<uint32_t>(inSets.size()), inSets.data());
}

void VulkanDescriptorSet::Create(Device* inVulkanDevice, uint32_t inCount, VulkanDescriptorSet** inSets)
{
	//std::vector<DescriptorSetLayout> layouts;
	//for (uint32_t index = 0; index < inCount; index++)
	//{
	//	inSets[index]->vulkanDevice = inVulkanDevice;
	//	layouts.push_back(inSets[index]->CreateLayout());
	//}

	//DescriptorPool pool;
	//std::vector<DescriptorSet> sets = Engine::GetRendererInstance()->GetDescriptorHeaps().AllocateSet({ layouts }, pool);

	//for (uint32_t index = 0; index < inCount; index++)
	//{
	//	inSets[index]->set = sets[index];
	//	inSets[index]->pool = pool;
	//}
}

void VulkanDescriptorSet::Destroy()
{
}

void VulkanDescriptorSet::SetDescriptors(const std::vector<CD3DX12_DESCRIPTOR_RANGE1>& inDescriptors)
{
	descriptors = inDescriptors;
}

//void VulkanDescriptorSet::SetBindings(const std::vector<DescriptorSetLayoutBinding>& inBindings)
//{
//	bindings = inBindings;
//}
//
//std::vector<DescriptorSetLayoutBinding> VulkanDescriptorSet::ProduceCustomBindings()
//{
//	return {};
//}
//
//DescriptorSetLayout& VulkanDescriptorSet::CreateLayout()
//{
//	if (bindings.size() == 0)
//	{
//		bindings = ProduceCustomBindings();
//	}
//
//	DescriptorSetLayoutCreateInfo layoutInfo;
//	layoutInfo.setBindingCount(static_cast<uint32_t>(bindings.size()));
//	layoutInfo.setPBindings(bindings.data());
//
//	layout = vulkanDevice->GetNativeDevice().createDescriptorSetLayout(layoutInfo);
//
//	return layout;
//}


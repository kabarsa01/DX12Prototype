#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include "DescriptorPool.h"

class Device;

class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet();
	virtual ~VulkanDescriptorSet();

	void Create(Device* inVulkanDevice);
	static void Create(Device* inVulkanDevice, std::vector<VulkanDescriptorSet*>& inSets);
	static void Create(Device* inVulkanDevice, uint32_t inCount, VulkanDescriptorSet** inSets);
	void Destroy();

	std::vector<CD3DX12_DESCRIPTOR_RANGE1>& GetDescriptors() { return descriptors; }
	CD3DX12_ROOT_PARAMETER1 GetTableRootParameter() { return tableRootParameter; }
	void SetDescriptors(const std::vector<CD3DX12_DESCRIPTOR_RANGE1>& inDescriptors);
//	void SetBindings(const std::vector<DescriptorSetLayoutBinding>& inBindings);
//	std::vector<DescriptorSetLayoutBinding>& GetBindings() { return bindings; }

//	operator bool() { return set; }
protected:
//	virtual std::vector<DescriptorSetLayoutBinding> ProduceCustomBindings();
private:
	Device* vulkanDevice;
	std::vector<CD3DX12_DESCRIPTOR_RANGE1> descriptors;
	CD3DX12_ROOT_PARAMETER1 tableRootParameter;
	DescriptorBlock descriptorBlock;
};

#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"

#include "render/DataStructures.h"
#include "objects/VulkanDescriptorSet.h"
#include "resources/BufferResource.h"
#include "objects/DescriptorPool.h"

class PerFrameData
{
public:
	PerFrameData();
	virtual ~PerFrameData();

	void Create(Device* inDevice);
	void Destroy();
	void UpdateBufferData();
	inline VulkanDescriptorSet& GetVulkanDescriptorSet() { return set; }
	inline CD3DX12_ROOT_PARAMETER1& GetRootParameter() { return descriptorTable; }
//	DescriptorSet& GetSet() { return set.GetSet(); }
//	DescriptorSetLayout& GetLayout() { return set.GetLayout(); }
private:
	Device* device;

	BufferResource shaderDataBuffer;
	BufferResource transformDataBuffer;
	CD3DX12_ROOT_PARAMETER1 descriptorTable;
	DescriptorBlock descriptorBlock;

	//DescriptorSetLayoutBinding shaderDataBinding;
	//DescriptorSetLayoutBinding transformDataBinding;

	//VulkanDescriptorSet set;
	//std::vector<WriteDescriptorSet> descriptorWrites;

	GlobalShaderData* globalShaderData;
	GlobalTransformData* globalTransformData;

	//std::vector<DescriptorSetLayoutBinding> ProduceBindings();
	//std::vector<WriteDescriptorSet> ProduceWrites(VulkanDescriptorSet& inSet);
	void GatherData();
};

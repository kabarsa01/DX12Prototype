#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include "DescriptorPool.h"
#include <vector>

using namespace Microsoft::WRL;

class Device;

class DescriptorHeaps
{
public:
	DescriptorHeaps();
	virtual ~DescriptorHeaps();

	void Create(Device* inDevice);
	void Destroy();

	inline std::vector<DescriptorPool>& GetShaderResourcePools() { return shaderResourcesPools; }
	inline std::vector<DescriptorPool>& GetRTVPools() { return RTVPools; }
	inline std::vector<DescriptorPool>& GetDSVPools() { return DSVPools; }

//	std::vector<DescriptorSet> AllocateSet(const std::vector<DescriptorSetLayout>& inLayouts, DescriptorPool& outPool);
private:
	Device* device;
	std::vector<DescriptorPool> shaderResourcesPools;
	std::vector<DescriptorPool> RTVPools;
	std::vector<DescriptorPool> DSVPools;
};

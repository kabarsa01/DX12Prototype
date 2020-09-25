#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include "DescriptorPool.h"
#include <vector>

using namespace Microsoft::WRL;

class Device;
class ResourceView;

class DescriptorHeaps
{
public:
	DescriptorHeaps();
	virtual ~DescriptorHeaps();

	void Create(Device* inDevice);
	void Destroy();

	inline std::vector<DescriptorPool>& GetCBV_SRV_UAVPools() { return CBV_SRV_UAVPools; }
	inline std::vector<DescriptorPool>& GetRTVPools() { return RTVPools; }
	inline std::vector<DescriptorPool>& GetDSVPools() { return DSVPools; }

	DescriptorBlock AllocateDescriptorsCBV_SRV_UAV(uint16_t inBlockSize);
	DescriptorBlock AllocateDescriptorsRTV(uint16_t inBlockSize);
	DescriptorBlock AllocateDescriptorsDSV(uint16_t inBlockSize);

	void ReleaseDescriptors(const DescriptorBlock& inBlock);
private:
	Device* device;
	std::vector<DescriptorPool> CBV_SRV_UAVPools;
	std::vector<DescriptorPool> RTVPools;
	std::vector<DescriptorPool> DSVPools;

	DescriptorBlock AllocateDescriptors(uint16_t inBlockSize, std::vector<DescriptorPool>& inPools, D3D12_DESCRIPTOR_HEAP_TYPE inType, uint16_t inHeapSize, bool inShaderVisible);
	void ReleaseDescriptors(const DescriptorBlock& inBlock);
};



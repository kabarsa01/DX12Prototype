#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

class Device;

class DescriptorHeaps
{
public:
	DescriptorHeaps();
	virtual ~DescriptorHeaps();

	void Create(Device* inDevice);
	void Destroy();

	inline ComPtr<ID3D12DescriptorHeap> GetShaderResourceHeap() { return shaderResourcesHeap; }
	inline ComPtr<ID3D12DescriptorHeap> GetSamplersHeap() { return samplersHeap; }
	inline ComPtr<ID3D12DescriptorHeap> GetRTVHeap() { return RTVHeap; }
	inline ComPtr<ID3D12DescriptorHeap> GetDSVHeap() { return DSVHeap; }

//	std::vector<DescriptorSet> AllocateSet(const std::vector<DescriptorSetLayout>& inLayouts, DescriptorPool& outPool);
private:
	Device* device;
	ComPtr<ID3D12DescriptorHeap> shaderResourcesHeap;
	ComPtr<ID3D12DescriptorHeap> samplersHeap;
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;
//	std::vector<DescriptorPool> pools;

//	DescriptorPool ConstructDescriptorPool();
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inType, uint32_t inDescriptorsCount);
};

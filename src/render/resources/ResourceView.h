#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "../objects/DescriptorPool.h"

class ResourceView
{
public:
	uint16_t blockOffset;
	DescriptorBlock descriptorBlock;

	ResourceView();
	ResourceView(uint16_t inBlockOffset, DescriptorBlock inDescriptorBlock);
	~ResourceView();
	inline operator CD3DX12_CPU_DESCRIPTOR_HANDLE() { return descriptorBlock.GetCpuHandle(blockOffset); }
	inline operator CD3DX12_GPU_DESCRIPTOR_HANDLE() { return descriptorBlock.GetGpuHandle(blockOffset); }

	static ResourceView CreateCBV(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex);

	static ResourceView CreateSRVTexture2D(ID3D12Resource* inResource, D3D12_TEX2D_SRV inSRV, DescriptorBlock& inBlock, uint16_t inIndex);
	static ResourceView CreateSRVTexture2D(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex);

	static ResourceView CreateRTVTexture2D(ID3D12Resource* inResource, D3D12_TEX2D_RTV inRTV, DescriptorBlock& inBlock, uint16_t inIndex);
	static ResourceView CreateRTVTexture2D(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex);

	static ResourceView CreateDSVTexture2D(ID3D12Resource* inResource, D3D12_TEX2D_DSV inDSV, DescriptorBlock& inBlock, uint16_t inIndex);
	static ResourceView CreateDSVTexture2D(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex);
private:
};
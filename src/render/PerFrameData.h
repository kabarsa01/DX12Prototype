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
	inline CD3DX12_ROOT_PARAMETER1& GetRootParameter() { return descriptorTable; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() { return descriptorBlock.gpuHandle; }
	inline DescriptorBlock& GetDescriptorBlock() { return descriptorBlock; }
private:
	Device* device;
	BufferResource shaderDataBuffer;
	BufferResource transformDataBuffer;
	D3D12_DESCRIPTOR_RANGE1 ranges[2];
	CD3DX12_ROOT_PARAMETER1 descriptorTable;
	DescriptorBlock descriptorBlock;

	GlobalShaderData* globalShaderData;
	GlobalTransformData* globalTransformData;

	void GatherData();
};

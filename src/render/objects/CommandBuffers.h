#pragma once

#include "Device.h"
#include <vector>
#include <map>
#include <d3d12.h>

using namespace Microsoft::WRL;

class CommandBuffers
{
public:
	CommandBuffers();
	virtual ~CommandBuffers();

	void Create(Device* inDevice, uint32_t inPoolsCount, uint32_t inCmdBuffersPerPool);
	void Destroy();

	ComPtr<ID3D12CommandAllocator> GetCommandPool(uint32_t inPoolIndex);
	ComPtr<ID3D12GraphicsCommandList> GetNextForPool(uint32_t inPoolIndex);
	ComPtr<ID3D12GraphicsCommandList> GetForPool(uint32_t inPoolIndex, uint32_t inBufferIndex);
private:
	Device* device;
	std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators;
	std::map<uint32_t, std::vector<ComPtr<ID3D12GraphicsCommandList>>> commandLists;

	std::vector<uint32_t> nextBuffer;

	uint32_t poolsCount;
	uint32_t cmdBuffersPerPool;

	ComPtr<ID3D12CommandAllocator> CreateAllocator(Device* inDevice, D3D12_COMMAND_LIST_TYPE inType);
	ComPtr<ID3D12GraphicsCommandList> CreateList(Device* inDevice, ComPtr<ID3D12CommandAllocator> inAllocator, D3D12_COMMAND_LIST_TYPE inType);
};


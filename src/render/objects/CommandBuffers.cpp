#include "CommandBuffers.h"
#include "utils/HelperUtils.h"
#include <d3d12.h>

CommandBuffers::CommandBuffers()
{

}

CommandBuffers::~CommandBuffers()
{

}

void CommandBuffers::Create(Device* inDevice, uint32_t inPoolsCount, uint32_t inCmdBuffersPerPool)
{
	device = inDevice;
	poolsCount = inPoolsCount;
	cmdBuffersPerPool = inCmdBuffersPerPool;

	//---------------------------------------------

	commandAllocators.resize(poolsCount);
	for (uint32_t allocatorIndex = 0; allocatorIndex < poolsCount; allocatorIndex++)
	{
		commandAllocators[allocatorIndex] = CreateAllocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		for (uint32_t listIndex = 0; listIndex < cmdBuffersPerPool; listIndex++)
		{
			commandLists[allocatorIndex].push_back(CreateList(inDevice, commandAllocators[allocatorIndex], D3D12_COMMAND_LIST_TYPE_DIRECT));
		}
	}

	nextBuffer = std::vector<uint32_t>(poolsCount, 0);
}

void CommandBuffers::Destroy()
{
	//if (device)
	//{
	//	device->GetDevice().freeCommandBuffers(transferPool, cmdBuffersPerPool, transferBuffers.data());
	//	device->GetDevice().destroyCommandPool(transferPool);

	//	for (uint32_t index = 0; index < poolsCount; index++)
	//	{
	//		device->GetDevice().freeCommandBuffers(pools[index], cmdBuffersPerPool, buffers[index].data());
	//		device->GetDevice().destroyCommandPool(pools[index]);
	//	}
	//}
}

ComPtr<ID3D12CommandAllocator> CommandBuffers::GetCommandPool(uint32_t inPoolIndex)
{
	return commandAllocators[inPoolIndex];
}

ComPtr<ID3D12GraphicsCommandList> CommandBuffers::GetNextForPool(uint32_t inPoolIndex)
{
	uint32_t bufferIndex = nextBuffer[inPoolIndex];
	nextBuffer[inPoolIndex] = (bufferIndex + 1) % cmdBuffersPerPool;
	return commandLists[inPoolIndex][bufferIndex];
}

ComPtr<ID3D12GraphicsCommandList> CommandBuffers::GetForPool(uint32_t inPoolIndex, uint32_t inBufferIndex)
{
	return commandLists[inPoolIndex][inBufferIndex];
}

ComPtr<ID3D12CommandAllocator> CommandBuffers::CreateAllocator(Device* inDevice, D3D12_COMMAND_LIST_TYPE inType)
{
	ComPtr<ID3D12CommandAllocator> allocator;
	ThrowIfFailed( inDevice->GetDevice()->CreateCommandAllocator(inType, IID_PPV_ARGS(&allocator)) );

	return allocator;
}

ComPtr<ID3D12GraphicsCommandList> CommandBuffers::CreateList(Device* inDevice, ComPtr<ID3D12CommandAllocator> inAllocator, D3D12_COMMAND_LIST_TYPE inType)
{
	ComPtr<ID3D12GraphicsCommandList> list;
	ThrowIfFailed( inDevice->GetDevice()->CreateCommandList(0, inType, inAllocator.Get(), nullptr, IID_PPV_ARGS(&list)) );
	ThrowIfFailed(list->Close());

	return list;
}


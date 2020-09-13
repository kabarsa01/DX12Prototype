#include "DeviceMemory.h"
#include "../objects/Device.h"

DeviceMemory::DeviceMemory()
{

}

DeviceMemory::~DeviceMemory()
{
}

void DeviceMemory::Allocate(Device* inDevice, uint64_t inSize, D3D12_HEAP_TYPE inHeapType)
{
	if (memoryHeap.Get() != nullptr)
	{
		return;
	}

	device = inDevice;

	D3D12_HEAP_PROPERTIES heapProps;
	heapProps.CreationNodeMask = 0;
	heapProps.VisibleNodeMask = 0;
	heapProps.Type = inHeapType;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	D3D12_HEAP_DESC heapDesc;
	heapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
	heapDesc.SizeInBytes = inSize;
	heapDesc.Properties = heapProps;

	device->GetNativeDevice()->CreateHeap(&heapDesc, IID_PPV_ARGS(&memoryHeap));

	size = inSize;
	heapType = inHeapType;
}

void DeviceMemory::Allocate(Device* inDevice)
{
	if (memoryHeap.Get() == nullptr)
	{
		Allocate(device, size, heapType);
	}
}

bool DeviceMemory::IsValid()
{
	return memoryHeap;
}

DeviceMemory::operator bool() const
{
	return memoryHeap.Get() == nullptr;
}






#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>

using namespace Microsoft::WRL;

class Device;

class DeviceMemory
{
public:
	DeviceMemory();
	virtual ~DeviceMemory();

	inline DeviceMemory& SetSize(uint64_t inSize) { size = inSize; }
	inline uint64_t GetSize() { return size; }
	inline DeviceMemory& SetHeapType(D3D12_HEAP_TYPE inHeaptype) { heapType = inHeaptype; }
	inline D3D12_HEAP_TYPE GetHeapType() { return heapType; }
	inline ComPtr<ID3D12Heap> GetHeap() { return memoryHeap; }

	void Allocate(Device* inDevice, uint64_t inSize, D3D12_HEAP_TYPE inHeapType);
	void Allocate(Device* inDevice);
	bool IsValid();

	operator bool() const;
protected:
	Device* device;
	ComPtr<ID3D12Heap> memoryHeap;
	uint64_t size;
	D3D12_HEAP_TYPE heapType;
};



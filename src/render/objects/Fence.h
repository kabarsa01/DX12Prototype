#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>

#if defined(max)
#undef max
#endif

#include <chrono>

using namespace Microsoft::WRL;

class Device;

// Specialized fence wrapper for utility needs, operating with one-to-one fence-event relationship,
// CPU side reset is used to get back to nonsignaled state
class Fence
{
public:
	Fence();
	~Fence();

	void Create(Device* inDevice);
	void Destroy();

	void Signal();
	void Signal(ComPtr<ID3D12CommandQueue> inQueue);
	void Wait(std::chrono::milliseconds inTimeout = std::chrono::milliseconds::max());
	void Wait(ComPtr<ID3D12CommandQueue> inQueue, std::chrono::milliseconds inTimeout = std::chrono::milliseconds::max());
	void Flush(ComPtr<ID3D12CommandQueue> inQueue);
	void Reset();
protected:
	Device* device;
	ComPtr<ID3D12Fence> d3dFence;
	HANDLE eventHandle;
	bool isValid = false;
};
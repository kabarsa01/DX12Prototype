#include "Fence.h"
#include "utils/HelperUtils.h"
#include "Device.h"

namespace {
	const uint64_t NON_SIGNALED_STATE = 0;
	const uint64_t SIGNALED_STATE = 1;
}

Fence::Fence()
	: isValid(false)
{

}

Fence::~Fence()
{

}

void Fence::Create(Device* inDevice)
{
	if (isValid)
	{
		return;
	}
	device = inDevice;
	ThrowIfFailed(device->GetNativeDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3dFence)));
	eventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

void Fence::Destroy()
{
	if (!isValid)
	{
		return;
	}
	::CloseHandle(eventHandle);
}

void Fence::Signal()
{
	ThrowIfFailed( d3dFence->Signal(SIGNALED_STATE) );
}

void Fence::Signal(ComPtr<ID3D12CommandQueue> inQueue)
{
	ThrowIfFailed( inQueue->Signal(d3dFence.Get(), SIGNALED_STATE) );
}

void Fence::Wait(std::chrono::milliseconds inTimeout /*= std::chrono::milliseconds::max()*/)
{
	if (d3dFence->GetCompletedValue() == NON_SIGNALED_STATE)
	{
		ThrowIfFailed(d3dFence->SetEventOnCompletion(SIGNALED_STATE, eventHandle));
		::WaitForSingleObject(eventHandle, static_cast<DWORD>(inTimeout.count()));
	}
}

void Fence::Wait(ComPtr<ID3D12CommandQueue> inQueue, std::chrono::milliseconds inTimeout /*= std::chrono::milliseconds::max()*/)
{
	if (d3dFence->GetCompletedValue() == NON_SIGNALED_STATE)
	{
		ThrowIfFailed( inQueue->Wait(d3dFence.Get(), SIGNALED_STATE) );
	}
}

void Fence::Flush(ComPtr<ID3D12CommandQueue> inQueue)
{
	Signal(inQueue);
	Wait();
}

void Fence::Reset()
{
	ThrowIfFailed( d3dFence->Signal(NON_SIGNALED_STATE) );
}


#include "Device.h"
#include "core/Engine.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "../memory/DeviceMemoryManager.h"
#include "../../utils/HelperUtils.h"

Device::Device()
{

}

Device::~Device()
{

}

void Device::Create(const char* inAppName, const char* inEngine, bool inValidationEnabled, HWND inHwnd)
{
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory4> dxgiFactory;
	UINT factoryFlags = 0;
#if defined(_DEBUG)
	factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory)));

	UINT adapterIndex = 0;
	SIZE_T dedicatedVideoMemory = 0;
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	for (UINT index = 0; dxgiFactory->EnumAdapters1(index, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND; index++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		hardwareAdapter->GetDesc1(&dxgiAdapterDesc1);

		bool isHardware = (dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
		if (isHardware)
		{
			bool isFeatureLevelSupported = SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr));
			bool hasMoreMemory = dxgiAdapterDesc1.DedicatedVideoMemory > dedicatedVideoMemory;

			if (isFeatureLevelSupported && hasMoreMemory)
			{
				dedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				adapterIndex = index;
			}
		}
	}

	ComPtr<IDXGIAdapter4> dxgiAdapter4;
	ThrowIfFailed(dxgiFactory->EnumAdapters1(adapterIndex, &hardwareAdapter));
	ThrowIfFailed(hardwareAdapter.As(&dxgiAdapter4));

	ThrowIfFailed( D3D12CreateDevice(dxgiAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) );

#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (device.As(&infoQueue))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
	}

	D3D12_MESSAGE_SEVERITY deniedSeverities[] = {
		D3D12_MESSAGE_SEVERITY_INFO
	};
	D3D12_MESSAGE_ID deniedIDs[] = {
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
	};

	D3D12_INFO_QUEUE_FILTER infoQueueFilter;
	infoQueueFilter.DenyList.NumSeverities = std::size(deniedSeverities);
	infoQueueFilter.DenyList.pSeverityList = deniedSeverities;
	infoQueueFilter.DenyList.NumIDs = std::size(deniedIDs);
	infoQueueFilter.DenyList.pIDList = deniedIDs;

	infoQueue->PushStorageFilter(&infoQueueFilter);
#endif

	directQueue = CreateCommandQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void Device::Destroy()
{
}

ComPtr<IDXGIAdapter4> Device::PickAdapter()
{

}

ComPtr<ID3D12CommandQueue> Device::CreateCommandQueue(ComPtr<ID3D12Device2> inDevice, D3D12_COMMAND_LIST_TYPE inType)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc.Type = inType;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	ComPtr<ID3D12CommandQueue> commandQueue;
	ThrowIfFailed( inDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)) );

	return commandQueue;
}




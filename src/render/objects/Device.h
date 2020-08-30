#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

class Device
{
public:
	Device();
	virtual ~Device();

	void Create(const char* inAppName, const char* inEngine, bool inValidationEnabled, HWND inHwnd);
	void Destroy();

	//Instance& GetInstance() { return instance; }
	//VulkanPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
	ComPtr<ID3D12Device2> GetDevice() { return device; }
	ComPtr<ID3D12CommandQueue> GetDirectQueue() { return directQueue; }
	//SurfaceKHR& GetSurface() { return surface; };
	//PipelineCache& GetPipelineCache() { return pipelineCache; };

	//Queue& GetGraphicsQueue() { return graphicsQueue; }
	//Queue& GetComputeQueue() { return computeQueue; }
	//Queue& GetPresentQueue() { return presentQueue; }
	//Queue& GetTransferQueue() { return transferQueue; }
	//uint32_t GetGraphicsQueueIndex() { return queueFamilyIndices.graphicsFamily.value(); }
	//uint32_t GetComputeQueueIndex() { return queueFamilyIndices.computeFamily.value(); }
	//uint32_t GetPresentQueueIndex() { return queueFamilyIndices.presentFamily.value(); }
	//uint32_t GetTransferQueueIndex() { return queueFamilyIndices.transferFamily.value(); }

//	operator Instance() { return instance; }
	operator ComPtr<ID3D12Device2>() { return device; }
	//operator PhysicalDevice() { return physicalDevice.GetDevice(); }
	//operator SurfaceKHR() { return surface; }
	//operator PipelineCache() { return pipelineCache; }
protected:
	//const std::vector<const char*> validationLayers = {
	//	"VK_LAYER_KHRONOS_validation"
	//};
	//std::vector<const char*> requiredExtensions = {
	//	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	//};

	ComPtr<ID3D12Device2> device;
	ComPtr<ID3D12CommandQueue> directQueue;

	//Instance instance;
	//VulkanPhysicalDevice physicalDevice;
	//Device device;
	//SurfaceKHR surface;

	//QueueFamilyIndices queueFamilyIndices;

	//Queue graphicsQueue;
	//Queue computeQueue;
	//Queue presentQueue;
	//Queue transferQueue;

//	PipelineCache pipelineCache;

	ComPtr<IDXGIAdapter4> PickAdapter();
	ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> inDevice, D3D12_COMMAND_LIST_TYPE inType);

//	bool CheckValidationLayerSupport();
//	VulkanPhysicalDevice PickPhysicalDevice(std::vector<PhysicalDevice>& inDevices);
//	int ScoreDeviceSuitability(VulkanPhysicalDevice& inPhysicalDevice);
//	int IsDeviceUsable(VulkanPhysicalDevice& inPhysicalDevice, SurfaceKHR& inSurface);
};




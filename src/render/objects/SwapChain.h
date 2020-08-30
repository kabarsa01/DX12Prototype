#pragma once

#include "Device.h"

#include <dxgi1_6.h>
#include <wrl.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

class SwapChain
{
public:
	SwapChain();
	virtual ~SwapChain();

	void Create(Device* inDevice, uint32_t inBuffersCount = 2);
	void Destroy();

	//void CreateForResolution(uint32_t inWidth, uint32_t inHeight);
	//void DestroyForResolution();

	uint32_t AcquireNextImage(bool& outBecameOutdated);
	bool Present();

	void WaitForPresentQueue();

	ComPtr<ID3D12Resource> GetImage() { return images[imageIndex]; }
	ComPtr<ID3D12Resource> GetPrevImage() { return images[prevImageIndex]; }
	uint32_t GetImageIndex() { return imageIndex; }
	uint32_t GetPrevImageIndex() { return prevImageIndex; }
	//Semaphore& GetImageAvailableSemaphore() { return imageAvailableSemaphores[prevImageIndex]; }
	//Semaphore& GetRenderingFinishedSemaphore() { return renderingFinishedSemaphores[imageIndex]; }
	ComPtr<ID3D12Fence> GetGraphicsQueueFence();
	ComPtr<ID3D12Fence> GetGraphicsQueuePrevFence();
	//Framebuffer& GetFramebuffer() { return framebuffers[imageIndex]; }
	//Framebuffer& GetFramebuffer(uint32_t inIndex) { return framebuffers[inIndex]; }
	//uint32_t GetFramebuffersCount() { return static_cast<uint32_t>(framebuffers.size()); }

	ComPtr<IDXGISwapChain4> GetSwapChain() { return swapChain4; }
	//RenderPass& GetRenderPass() { return renderPass; }
	//Queue& GetPresentQueue() { return presentQueue; }
	//Extent2D& GetExtent() { return extent; }
	//Format& GetImageFormat() { return imageFormat; }

	//operator SwapchainKHR() { return swapChain4; }
	//operator bool() { return swapChain4; }
	//operator Queue() { return presentQueue; }
	//operator Extent2D() { return extent; }
private:
	Device* device;
	ComPtr<IDXGISwapChain4> swapChain4;
	std::vector< ComPtr<ID3D12Resource> > images;
	std::vector< ComPtr<ID3D12Fence> > fences;

	uint32_t buffersCount = 2;

	//std::vector<ImageView> imageViews;
	//std::vector<Framebuffer> framebuffers;

	//std::vector<Semaphore> renderingFinishedSemaphores;
	//std::vector<Semaphore> imageAvailableSemaphores;

	//SwapChainSupportDetails swapChainSupportDetails;
	//SurfaceFormatKHR surfaceFormat;
	//PresentModeKHR presentMode;
	//Format imageFormat;
	//Extent2D extent;

	//Queue presentQueue;

	uint32_t imageIndex = 0;
	uint32_t prevImageIndex = 0;

	bool CheckTearingSupport();
	void CreateRTVs();
	void CreateFences();

	//void CreateRenderPass();
	//void DestroyRenderPass();
	//void DestroyRTV();
	//void CreateFramebuffers();
	//void DestroyFramebuffers();
};

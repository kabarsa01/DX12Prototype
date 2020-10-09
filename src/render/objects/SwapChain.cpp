#include "SwapChain.h"
#include "core/Engine.h"
#include <algorithm>
#include "utils/HelperUtils.h"
#include "../Renderer.h"
#include "d3dx12.h"
#include "../resources/ResourceView.h"

SwapChain::SwapChain()
{

}

SwapChain::~SwapChain()
{

}

void SwapChain::Create(Device* inDevice, uint32_t inBuffersCount/* = 2*/)
{
	device = inDevice;
	buffersCount = std::max<uint32_t>(inBuffersCount, 2);
	isTearingSupported = CheckTearingSupport();

	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT factoryFlags = 0;
#if defined(_DEBUG)
	factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed( CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory4)) );

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	Renderer* renderInst = Engine::GetRendererInstance();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = renderInst->GetWidth();
	swapChainDesc.Height = renderInst->GetHeight();
	swapChainDesc.BufferCount = buffersCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	HWND hWnd = Engine::GetInstance()->GetWin32Window();

	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed( dxgiFactory4->CreateSwapChainForHwnd(
		device->GetDirectQueue().Get(), 
		hWnd, 
		&swapChainDesc, 
		nullptr, 
		nullptr, 
		swapChain1.GetAddressOf()) );
	ThrowIfFailed(swapChain1.As(&swapChain4));
	//ThrowIfFailed( dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER) );

	CreateRTVs();
	CreateFences();
	//imageIndex = 0;
	//prevImageIndex = buffersCount - 1;
}

void SwapChain::Destroy()
{
}


//void SwapChain::CreateForResolution(uint32_t inWidth, uint32_t inHeight)
//{
//	Device& device = device->GetDevice();
//
//	swapChainSupportDetails = device->GetPhysicalDevice().QuerySwapChainSupport(*device);
//	surfaceFormat = ChooseSurfaceFormat(swapChainSupportDetails.formats);
//	presentMode = ChooseSwapChainPresentMode(swapChainSupportDetails.presentModes);
//	imageFormat = surfaceFormat.format;
//
//	DestroyRenderPass();
//	CreateRenderPass();
//
//	//int windowWidth, windowHeight;
//	//GLFWwindow* window = Engine::GetInstance()->GetGlfwWindow();
//	//glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
//	extent = ChooseSwapChainExtent(swapChainSupportDetails.capabilities, inWidth, inHeight);
//	SurfaceCapabilitiesKHR& capabilities = swapChainSupportDetails.capabilities;
//	//uint32_t imageCount = capabilities.minImageCount + 1;
//	//if (capabilities.maxImageCount > 0 && (imageCount > capabilities.maxImageCount))
//	//{
//	//	imageCount = capabilities.maxImageCount;
//	//}
//
//	//SwapchainCreateInfoKHR createInfo;
//	//createInfo.setSurface(*device);
//	//createInfo.setMinImageCount(backBuffersCount);
//	//createInfo.setImageFormat(surfaceFormat.format);
//	//createInfo.setImageColorSpace(surfaceFormat.colorSpace);
//	//createInfo.setImageExtent(extent);
//	//createInfo.setImageUsage(ImageUsageFlagBits::eColorAttachment);// ImageUsageFlagBits::eTransferDst or ImageUsageFlagBits::eDepthStencilAttachment
//	//createInfo.setImageArrayLayers(1);
//	//createInfo.setPreTransform(capabilities.currentTransform);
//	//createInfo.setCompositeAlpha(CompositeAlphaFlagBitsKHR::eOpaque);
//	//createInfo.setPresentMode(presentMode);
//	//createInfo.setClipped(VK_TRUE);
//	//createInfo.setOldSwapchain(SwapchainKHR());
//
//	//QueueFamilyIndices queueFamilyIndices = device->GetPhysicalDevice().GetCachedQueueFamiliesIndices();
//	//if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily)
//	//{
//	//	uint32_t familyIndices[] = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };
//	//	createInfo.setImageSharingMode(SharingMode::eConcurrent);
//	//	createInfo.setQueueFamilyIndexCount(2);
//	//	createInfo.setPQueueFamilyIndices(familyIndices);
//	//}
//	//else
//	//{
//	//	uint32_t familyIndices[] = { queueFamilyIndices.graphicsFamily.value() };
//	//	createInfo.setImageSharingMode(SharingMode::eExclusive);
//	//	createInfo.setQueueFamilyIndexCount(1);
//	//	createInfo.setPQueueFamilyIndices(familyIndices);
//	//}
//
//	//swapChain4 = device.createSwapchainKHR(createInfo);
//	//images = device.getSwapchainImagesKHR(swapChain4);
//
//	
//}
//
//void SwapChain::DestroyForResolution()
//{
//	DestroyRenderPass();
//	DestroyFramebuffers();
//	DestroyRTV();
//
//	if (swapChain4)
//	{
//		device->GetDevice().destroySwapchainKHR(swapChain4);
//	}
//}

uint32_t SwapChain::AcquireNextImage(bool& outBecameOutdated)
{
	outBecameOutdated = false;
//	prevImageIndex = imageIndex;
//
//	ResultValue<uint32_t> imageIndexResult = ResultValue<uint32_t>(Result::eSuccess, imageIndex);
//
//	try
//	{
//		imageIndexResult = device.acquireNextImageKHR(swapChain4, UINT64_MAX, imageAvailableSemaphores[imageIndex], Fence());
//	}
//	catch (std::exception exc)
//	{
//	}
//
//	if (imageIndexResult.result == Result::eErrorOutOfDateKHR || imageIndexResult.result == Result::eErrorIncompatibleDisplayKHR)
//	{
//		outBecameOutdated = true;
////		return 0;
//	}
//	else if (imageIndexResult.result != Result::eSuccess && imageIndexResult.result != Result::eSuboptimalKHR)
//	{
//		throw std::runtime_error("failed to acquire swap chain image");
//	}
//	imageIndex = imageIndexResult.value;
//
//	device->GetDevice().waitForFences(1, &cmdBuffersFences[imageIndex], VK_TRUE, UINT64_MAX);
//	device->GetDevice().resetFences(1, &cmdBuffersFences[imageIndex]);

	return swapChain4->GetCurrentBackBufferIndex();
}

bool SwapChain::Present(bool inVSync)
{
	UINT syncInterval = inVSync ? 1 : 0;
	UINT presentFlags = isTearingSupported && !inVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(swapChain4->Present(syncInterval, presentFlags));
	//SwapchainKHR swapChains[] = { swapChain4 };
	//PresentInfoKHR presentInfo;
	//presentInfo.setWaitSemaphoreCount(1);
	//presentInfo.setPWaitSemaphores(&renderingFinishedSemaphores[imageIndex]);
	//presentInfo.setSwapchainCount(1);
	//presentInfo.setPSwapchains(swapChains);
	//presentInfo.setPImageIndices(&imageIndex);
	//presentInfo.setPResults(nullptr);

	//Result presentResult = Result::eSuccess;
	//try
	//{
	//	presentResult = presentQueue.presentKHR(presentInfo);
	//}
	//catch (std::exception)
	//{
	//}

	//if (presentResult == Result::eErrorOutOfDateKHR || presentResult == Result::eSuboptimalKHR)
	//{
	//	return false;
	//}

	return true;
}

void SwapChain::WaitForPresentQueue()
{
//	presentQueue.waitIdle();
}

Fence SwapChain::GetCurrentFence()
{
	return fences[swapChain4->GetCurrentBackBufferIndex()];
}

void SwapChain::SignalFences()
{
	for (uint32_t index = 0; index < fences.size(); index++)
	{
		fences[index].Signal();
	}
}

void SwapChain::ResetFences()
{
	for (uint32_t index = 0; index < fences.size(); index++)
	{
		fences[index].Reset();
	}
}

//Fence SwapChain::GetPrevFence()
//{
//	return fences[prevImageIndex];
//}

bool SwapChain::CheckTearingSupport()
{
	BOOL allowTearing = FALSE;

	ComPtr<IDXGIFactory4> dxgiFactory4;
	if ( SUCCEEDED( CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory4)) ) )
	{
		ComPtr<IDXGIFactory5> dxgiFactory5;
		if (dxgiFactory4.As(&dxgiFactory5))
		{
			if (FAILED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
			{
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}
//
//void SwapChain::CreateRenderPass()
//{
//	AttachmentDescription colorAttachment;
//	colorAttachment.setFormat(imageFormat);
//	colorAttachment.setSamples(SampleCountFlagBits::e1);
//	colorAttachment.setLoadOp(AttachmentLoadOp::eClear);
//	colorAttachment.setStoreOp(AttachmentStoreOp::eStore);
//	colorAttachment.setStencilLoadOp(AttachmentLoadOp::eDontCare);
//	colorAttachment.setStencilStoreOp(AttachmentStoreOp::eDontCare);
//	colorAttachment.setInitialLayout(ImageLayout::eUndefined);
//	colorAttachment.setFinalLayout(ImageLayout::eColorAttachmentOptimal);
//
//	AttachmentReference colorAttachmentRef;
//	colorAttachmentRef.setAttachment(0);
//	colorAttachmentRef.setLayout(ImageLayout::eColorAttachmentOptimal);
//
//	SubpassDescription subpassDesc;
//	subpassDesc.setPipelineBindPoint(PipelineBindPoint::eGraphics);
//	subpassDesc.setColorAttachmentCount(1);
//	subpassDesc.setPColorAttachments(&colorAttachmentRef); // layout(location = |index| ) out vec4 outColor references attachmant by index
//
//	SubpassDependency subpassDependency;
//	subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
//	subpassDependency.setDstSubpass(0);
//	subpassDependency.setSrcStageMask(PipelineStageFlagBits::eColorAttachmentOutput);
//	subpassDependency.setSrcAccessMask(AccessFlags());
//	subpassDependency.setDstStageMask(PipelineStageFlagBits::eColorAttachmentOutput);
//	subpassDependency.setDstAccessMask(AccessFlagBits::eColorAttachmentRead | AccessFlagBits::eColorAttachmentWrite);
//
//	RenderPassCreateInfo renderPassInfo;
//	renderPassInfo.setAttachmentCount(1);
//	renderPassInfo.setPAttachments(&colorAttachment);
//	renderPassInfo.setSubpassCount(1);
//	renderPassInfo.setPSubpasses(&subpassDesc);
//	renderPassInfo.setDependencyCount(1);
//	renderPassInfo.setPDependencies(&subpassDependency);
//
//	renderPass = device->GetDevice().createRenderPass(renderPassInfo);
//}
//
//void SwapChain::DestroyRenderPass()
//{
//	if (renderPass && device)
//	{
//		device->GetDevice().destroyRenderPass(renderPass);
//		renderPass = nullptr;
//	}
//}

void SwapChain::CreateRTVs()
{
	images.resize(buffersCount);
	views.resize(buffersCount);

	DescriptorHeaps& heaps = Engine::GetRendererInstance()->GetDescriptorHeaps();
	RTVs = heaps.AllocateDescriptorsRTV(buffersCount);

	for (uint32_t index = 0; index < buffersCount; index++)
	{
		ComPtr<ID3D12Resource> buffer;
		ThrowIfFailed(swapChain4->GetBuffer(index, IID_PPV_ARGS(&buffer)));

		images[index] = buffer;
		views[index] = ResourceView::CreateRTVTexture2D(buffer.Get(), RTVs, index);
	}
}

void SwapChain::CreateFences()
{
	DestroyFences();
	fences.resize(buffersCount);

	for (uint32_t index = 0; index < buffersCount; index++)
	{
		Fence fence;
		fence.Create(device);

		fences[index] = fence;
	}
}

void SwapChain::DestroyFences()
{
	for (uint32_t index = 0; index < fences.size(); index++)
	{
		fences[index].Destroy();
	}
}

//
//void SwapChain::DestroyRTV()
//{
//	for (uint32_t index = 0; index < imageViews.size(); index++)
//	{
//		device->GetDevice().destroyImageView(imageViews[index]);
//	}
//	imageViews.clear();
//}
//
//void SwapChain::CreateFramebuffers()
//{
//	framebuffers.resize(backBuffersCount);
//	for (int index = 0; index < imageViews.size(); index++)
//	{
//		ImageView attachments[] = { imageViews[index] };
//
//		FramebufferCreateInfo framebufferInfo;
//		framebufferInfo.setRenderPass(renderPass);
//		framebufferInfo.setAttachmentCount(1);
//		framebufferInfo.setPAttachments(attachments);
//		framebufferInfo.setWidth(extent.width);
//		framebufferInfo.setHeight(extent.height);
//		framebufferInfo.setLayers(1);
//
//		framebuffers[index] = device->GetDevice().createFramebuffer(framebufferInfo);
//	}
//}
//
//void SwapChain::DestroyFramebuffers()
//{
//	for (uint32_t index = 0; index < framebuffers.size(); index++)
//	{
//		device->GetDevice().destroyFramebuffer(framebuffers[index]);
//	}
//	framebuffers.clear();
//}
//
//SurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<SurfaceFormatKHR>& inFormats)
//{
//	for (const SurfaceFormatKHR& surfaceFormat : inFormats)
//	{
//		if (surfaceFormat.format == Format::eR8G8B8A8Unorm && surfaceFormat.colorSpace == ColorSpaceKHR::eSrgbNonlinear)
//		{
//			return surfaceFormat;
//		}
//	}
//
//	return inFormats[0];
//}
//
//PresentModeKHR SwapChain::ChooseSwapChainPresentMode(const std::vector<PresentModeKHR>& inPresentModes)
//{
//	for (const PresentModeKHR& presentMode : inPresentModes)
//	{
//		if (presentMode == PresentModeKHR::eMailbox)
//		{
//			return presentMode;
//		}
//	}
//
//	return PresentModeKHR::eFifo;
//}
//
//Extent2D SwapChain::ChooseSwapChainExtent(const SurfaceCapabilitiesKHR& inCapabilities, uint32_t inWidth, uint32_t inHeight)
//{
//	if (inCapabilities.currentExtent.width == UINT32_MAX)
//	{
//		return inCapabilities.currentExtent;
//	}
//	else
//	{
//		Extent2D extent;
//
//		extent.setWidth(std::clamp<int>(inWidth, inCapabilities.minImageExtent.width, inCapabilities.maxImageExtent.width));
//		extent.setHeight(std::clamp<int>(inHeight, inCapabilities.minImageExtent.height, inCapabilities.maxImageExtent.height));
//
//		return extent;
//	}
//}



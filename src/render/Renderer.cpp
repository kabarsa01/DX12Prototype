#include "render/Renderer.h"
#include <iostream>
#include <map>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3native.h>
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include "core/Engine.h"
#include <vector>
#include <algorithm>
#include "shader/Shader.h"
#include <array>
#include "scene/camera/CameraComponent.h"
#include "scene/mesh/MeshComponent.h"
#include "scene/SceneObjectComponent.h"
#include "scene/Transform.h"
#include "scene/SceneObjectBase.h"
#include "DataStructures.h"
#include "TransferList.h"
#include "data/DataManager.h"
#include "PerFrameData.h"
#include "passes/GBufferPass.h"
#include "passes/ZPrepass.h"
#include "passes/PostProcessPass.h"
#include "PipelineRegistry.h"
#include "passes/DeferredLightingPass.h"
#include "passes/LightClusteringPass.h"
#include "objects/SwapChain.h"
#include "utils/HelperUtils.h"
#include "GlobalSamplers.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Init()
{
	GLFWwindow* window = Engine::GetInstance()->GetGlfwWindow();
	glfwGetFramebufferSize(window, &width, &height);

	HWND hWnd = glfwGetWin32Window(Engine::GetInstance()->GetGlfwWindow());
	device.Create("DX12Renderer", "MiniEngine", hWnd);
	descriptorHeaps.Create(&device);
	swapChain.Create(&device, 3);
	swapChain.SignalFences();
//	swapChain.CreateForResolution(width, height);
	commandBuffers.Create(&device, 3, 1);

	GlobalSamplers::GetInstance()->Create(&device);

	perFrameData = new PerFrameData();
	perFrameData->Create(&device);

	zPrepass = new ZPrepass(HashString("ZPrepass"));
	zPrepass->SetResolution(width, height);
	zPrepass->Create();
	lightClusteringPass = new LightClusteringPass(HashString("LightClusteringPass"));
	lightClusteringPass->Create();
	gBufferPass = new GBufferPass(HashString("GBufferPass"));
	gBufferPass->SetExternalDepth(zPrepass->GetDepthAttachment());
	gBufferPass->SetResolution(width, height);
	gBufferPass->Create();
	deferredLightingPass = new DeferredLightingPass(HashString("DeferredLightingPass"));
	deferredLightingPass->SetResolution(width, height);
	deferredLightingPass->Create();
	postProcessPass = new PostProcessPass(HashString("PostProcessPass"));
	postProcessPass->SetResolution(width, height);
	postProcessPass->Create();
}

void Renderer::RenderFrame()
{
	if (framebufferResized)
	{
		OnResolutionChange();
		framebufferResized = false;
		return;
	}

	// waiting for the current frame to be available
	Fence currentFence = swapChain.GetCurrentFence();
	currentFence.Wait();
	// resetting our fence object
	currentFence.Reset();

	uint32_t imageIndex = swapChain.GetCurrentImageIndex();
	//if (outdated)
	//{
	//	OnResolutionChange();
	//	return;
	//}

	perFrameData->UpdateBufferData();

	ComPtr<ID3D12CommandAllocator> cmdAllocator = commandBuffers.GetCommandPool(imageIndex);
	ComPtr<ID3D12GraphicsCommandList> cmdList = commandBuffers.GetForPool(imageIndex);

	cmdAllocator->Reset();
	cmdList->Reset(cmdAllocator.Get(), nullptr);

	//CommandBufferBeginInfo beginInfo;
	//beginInfo.setFlags(CommandBufferUsageFlagBits::eSimultaneousUse);
	//beginInfo.setPInheritanceInfo(nullptr);

	//// begin command buffer record
	//cmdList.begin(beginInfo);
	// copy new data
	TransferResources(cmdList);

	// render passes
	// z prepass
	zPrepass->RecordCommands(cmdList);
	//--------------------------------------------------------
	lightClusteringPass->RecordCommands(cmdList);
	//--------------------------------------------------------
	// gbuffer pass
	gBufferPass->RecordCommands(cmdList);
	//--------------------------------------------------------
	// deferred lighting pass
	deferredLightingPass->RecordCommands(cmdList);
	//--------------------------------------------------------
	// post process pass
	postProcessPass->RecordCommands(cmdList);

	//CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//	swapChain.GetCurrentImage().Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_PRESENT
	//);
//	cmdList->ResourceBarrier(1, &presentBarrier);
	// end commands recording
	ThrowIfFailed(cmdList->Close());

	ComPtr<ID3D12CommandQueue> queue = device.GetDirectQueue();
	ID3D12CommandList* lists[] = { cmdList.Get() };
	queue->ExecuteCommandLists(static_cast<UINT>(std::size(lists)), lists);
	currentFence.Signal(queue);
	swapChain.Present(true);

	currentFence.Wait();
	// resetting our fence object
	currentFence.Reset();
	currentFence.Signal();
}

void Renderer::WaitForDevice()
{
	swapChain.WaitForFences();
}

void Renderer::Cleanup()
{
	swapChain.WaitForFences();

	postProcessPass->Destroy();
	delete postProcessPass;
	deferredLightingPass->Destroy();
	delete deferredLightingPass;
	gBufferPass->Destroy();
	delete gBufferPass;
	zPrepass->Destroy();
	delete zPrepass;
	lightClusteringPass->Destroy();
	delete lightClusteringPass;

	ScenePtr scene = Engine::GetSceneInstance();
//	MeshComponentPtr meshComp = scene->GetSceneComponent<MeshComponent>();
//	meshComp->meshData->DestroyBuffers();

	perFrameData->Destroy();
	delete perFrameData;

	PipelineRegistry::GetInstance()->DestroyPipelines(&device);

	commandBuffers.Destroy();
	swapChain.Destroy();
	descriptorHeaps.Destroy();
	device.Destroy();
}

void Renderer::SetResolution(int inWidth, int inHeight)
{
	//width = inWidth;
	//height = inHeight;

	framebufferResized = true;
}

int Renderer::GetWidth() const
{
	return width;
}

int Renderer::GetHeight() const
{
	return height;
}

void Renderer::OnResolutionChange()
{
	//device.GetNativeDevice().waitIdle();

	//GLFWwindow* window = Engine::GetInstance()->GetGlfwWindow();
	//glfwGetFramebufferSize(window, &width, &height);

	//postProcessPass->Destroy();
	//delete postProcessPass;
	//swapChain.DestroyForResolution();

	//swapChain.CreateForResolution(width, height);
	//postProcessPass = new PostProcessPass("PostProcessPass");
	//postProcessPass->SetResolution(width, height);
	//postProcessPass->Create();
}

void Renderer::TransferResources(ComPtr<ID3D12GraphicsCommandList> inCmdList)
{
	TransferList* TL = TransferList::GetInstance();

	// get new resources to copy
	std::vector<BufferResource*> buffers = TL->GetBuffers();
	std::vector<ImageResource*> images = TL->GetImages();
	TL->ClearBuffers();
	TL->ClearImages();

	if ( (buffers.size() == 0) && (images.size() == 0) )
	{
		return;
	}

	// buffers
	std::vector<D3D12_RESOURCE_BARRIER> buffersBarriers;
	for (BufferResource* buffer : buffers)
	{
		inCmdList->CopyResource(*buffer, * buffer->CreateStagingBuffer());
		buffersBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			*buffer, 
			D3D12_RESOURCE_STATE_COPY_DEST, 
			D3D12_RESOURCE_STATE_COMMON | D3D12_RESOURCE_STATE_INDEX_BUFFER | D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}

	// images
	// prepare memory barriers first
	std::vector<D3D12_RESOURCE_BARRIER> beforeTransferBarriers;
	std::vector<D3D12_RESOURCE_BARRIER> afterTransferBarriers;
	beforeTransferBarriers.resize(images.size());
	afterTransferBarriers.resize(images.size());
	for (uint32_t index = 0; index < images.size(); index++)
	{
		// images are initially created in COPY_DST state
//		beforeTransferBarriers[index] = CD3DX12_RESOURCE_BARRIER::Transition(*images[index], D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		afterTransferBarriers[index] = CD3DX12_RESOURCE_BARRIER::Transition(*images[index], D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

//	inCmdList->ResourceBarrier(static_cast<uint32_t>(beforeTransferBarriers.size()), beforeTransferBarriers.data());

	//submit copy
	for (ImageResource* image : images)
	{
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
		UINT numRows;
		UINT64 rowSizeBytes;
		UINT64 totalBytes;
		device.GetNativeDevice()->GetCopyableFootprints(&image->GetResource()->GetDesc(), 0, 1, 0, &footprint, &numRows, &rowSizeBytes, &totalBytes);

		D3D12_TEXTURE_COPY_LOCATION srcLoc;
		srcLoc.pResource = * image->GetStagingBuffer();
		srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLoc.PlacedFootprint = footprint;
		srcLoc.SubresourceIndex = 0;
		D3D12_TEXTURE_COPY_LOCATION dstLoc;
		dstLoc.pResource = *image;
		dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLoc.SubresourceIndex = 0;

		inCmdList->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);
	}

//	GenerateMips(inCmdBuffer, images);

	// final barriers for buffers and images
	if (afterTransferBarriers.size() > 0)
	{
		inCmdList->ResourceBarrier(static_cast<uint32_t>(afterTransferBarriers.size()), afterTransferBarriers.data());
	}
	if (buffersBarriers.size() > 0)
	{
		inCmdList->ResourceBarrier(static_cast<uint32_t>(buffersBarriers.size()), buffersBarriers.data());
	}
}




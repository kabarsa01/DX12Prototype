#pragma once

#include <memory>
#include <optional>
#include <core/ObjectBase.h>
#include "glm/fwd.hpp"
#include <set>
#include "objects/Device.h"
#include "objects/SwapChain.h"
#include "objects/CommandBuffers.h"
#include "memory/DeviceMemoryManager.h"
#include "resources/ImageResource.h"
#include "objects/DescriptorHeaps.h"

//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------

class PerFrameData;
class LightClusteringPass;
class ZPrepass;
class GBufferPass;
class DeferredLightingPass;
class PostProcessPass;

//=======================================================================================================
//=======================================================================================================

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void Init();
	void RenderFrame();
	void WaitForDevice();
	void Cleanup();

	void SetResolution(int InWidth, int InHeight);
	int GetWidth() const;
	int GetHeight() const;

	inline Device& GetDevice() { return device; }
	inline SwapChain& GetSwapChain() { return swapChain; }
	inline uint32_t GetBuffersCount() { return swapChain.GetBuffersCount(); }
	inline CommandBuffers& GetCommandBuffers() { return commandBuffers; }
	inline DescriptorHeaps& GetDescriptorHeaps() { return descriptorHeaps; }
	inline ComPtr<ID3D12CommandQueue> GetDirectQueue() { return device.GetDirectQueue(); }

	PerFrameData* GetPerFrameData() { return perFrameData; }
	ZPrepass* GetZPrepass() { return zPrepass; }
	LightClusteringPass* GetLightClusteringPass() { return lightClusteringPass; }
	GBufferPass* GetGBufferPass() { return gBufferPass; }
	DeferredLightingPass* GetDeferredLightingPass() { return deferredLightingPass; }
protected:
private:
	// TEMP
	BufferResource uniformBuffer;
	//======================= VARS ===============================
	uint32_t version;
	int width = 1600;
	int height = 900;
	bool framebufferResized = false;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	Device device;
	DescriptorHeaps descriptorHeaps;
	SwapChain swapChain;
	CommandBuffers commandBuffers;

	PerFrameData* perFrameData;

	ZPrepass* zPrepass;
	LightClusteringPass* lightClusteringPass;
	GBufferPass* gBufferPass;
	DeferredLightingPass* deferredLightingPass;
	PostProcessPass* postProcessPass;

	//==================== METHODS ===============================

	void TransferResources(ComPtr<ID3D12GraphicsCommandList> inCmdList);
	//void GenerateMips(CommandBuffer& inCmdBuffer, std::vector<VulkanImage*>& inImages);
	void OnResolutionChange();
};




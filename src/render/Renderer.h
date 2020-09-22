#pragma once

#include <memory>
#include <optional>
#include <core/ObjectBase.h>
#include "glm/fwd.hpp"
#include <set>
#include "data/MeshData.h"
#include "objects/VulkanPhysicalDevice.h"
#include "objects/Device.h"
#include "objects/SwapChain.h"
#include "objects/CommandBuffers.h"
#include "memory/DeviceMemoryManager.h"
#include "resources/ImageResource.h"
#include "objects/DescriptorHeaps.h"

// pre-build batch to compile all our shaders
//
//call :treeProcess
//goto : eof
//
//: treeProcess
//rem Do whatever you want here over the files of this subdir, for example :
//for %%f in(*.vert *.tesc *.tese *.geom *.frag *.comp) do glslangValidator - V % %f - o % %~nf.spv
//for / D % %d in(*) do (
//	cd %%d
//	call : treeProcess
//	cd ..
//)
//exit / b

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

	Device& GetDevice();
	SwapChain& GetSwapChain();
	CommandBuffers& GetCommandBuffers();
	DescriptorHeaps& GetDescriptorHeaps();
//	Queue GetGraphicsQueue();

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

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	Device device;
	SwapChain swapChain;
	CommandBuffers commandBuffers;
	DescriptorHeaps descriptorPools;
//	Viewport viewport;

	PerFrameData* perFrameData;

	ZPrepass* zPrepass;
	LightClusteringPass* lightClusteringPass;
	GBufferPass* gBufferPass;
	DeferredLightingPass* deferredLightingPass;
	PostProcessPass* postProcessPass;

	//==================== METHODS ===============================

	//void TransferResources(CommandBuffer& inCmdBuffer, uint32_t inQueueFamilyIndex);
	//void GenerateMips(CommandBuffer& inCmdBuffer, std::vector<VulkanImage*>& inImages);
	void OnResolutionChange();
};




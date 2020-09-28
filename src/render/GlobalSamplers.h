#pragma once

#include <d3d12.h>
#include "d3dx12.h"

#include "objects/Device.h"

// samplers to be used as immutable samplers
class GlobalSamplers
{
public:
	D3D12_STATIC_SAMPLER_DESC repeatLinearMipLinear;
	D3D12_STATIC_SAMPLER_DESC repeatMirrorLinearMipLinear;
	D3D12_STATIC_SAMPLER_DESC borderBlackLinearMipLinear;
	D3D12_STATIC_SAMPLER_DESC borderWhiteLinearMipLinear;

	static GlobalSamplers* GetInstance() { return instance; }

	void Create(Device* inVulkanDevice);
	void Destroy();

	inline void SetMipLodBias(float inMipLodBias) { mipLodBias = inMipLodBias; }
	inline uint32_t GetSamplersCounter() { return samplersDesc.size(); }
	inline D3D12_STATIC_SAMPLER_DESC* GetSamplersDescriptions() { return samplersDesc.data(); }
private:
	static GlobalSamplers* instance;

	Device* vulkanDevice;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplersDesc;
	float mipLodBias;

	GlobalSamplers();
	GlobalSamplers(const GlobalSamplers& inOther);
	virtual ~GlobalSamplers();
	void operator=(const GlobalSamplers& inOther);
};

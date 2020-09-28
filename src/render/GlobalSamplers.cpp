#include "GlobalSamplers.h"

GlobalSamplers* GlobalSamplers::instance = new GlobalSamplers();

GlobalSamplers::GlobalSamplers()
	: mipLodBias(0.0f)
{
}

GlobalSamplers::GlobalSamplers(const GlobalSamplers& inOther)
{
}

GlobalSamplers::~GlobalSamplers()
{
}

void GlobalSamplers::operator=(const GlobalSamplers& inOther)
{
}

void GlobalSamplers::Create(Device* inVulkanDevice)
{
	if (samplersDesc.size() > 0)
	{
		return;
	}

	vulkanDevice = inVulkanDevice;

	uint32_t shaderRegisterCounter = 0;

	repeatLinearMipLinear.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	repeatLinearMipLinear.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	repeatLinearMipLinear.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	repeatLinearMipLinear.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	repeatLinearMipLinear.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	repeatLinearMipLinear.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	repeatLinearMipLinear.MaxAnisotropy = 16;
	repeatLinearMipLinear.MaxLOD = D3D12_FLOAT32_MAX;
	repeatLinearMipLinear.MinLOD = 0.0f;
	repeatLinearMipLinear.MipLODBias = mipLodBias;
	repeatLinearMipLinear.RegisterSpace = 0;
	repeatLinearMipLinear.ShaderRegister = shaderRegisterCounter++;
	repeatLinearMipLinear.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	samplersDesc.push_back(repeatLinearMipLinear);

	repeatMirrorLinearMipLinear = repeatLinearMipLinear;
	repeatMirrorLinearMipLinear.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	repeatMirrorLinearMipLinear.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	repeatMirrorLinearMipLinear.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	repeatMirrorLinearMipLinear.ShaderRegister = shaderRegisterCounter++;

	samplersDesc.push_back(repeatMirrorLinearMipLinear);

	borderBlackLinearMipLinear = repeatLinearMipLinear;
	borderBlackLinearMipLinear.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	borderBlackLinearMipLinear.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	borderBlackLinearMipLinear.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	borderBlackLinearMipLinear.ShaderRegister = shaderRegisterCounter++;

	samplersDesc.push_back(borderBlackLinearMipLinear);

	borderWhiteLinearMipLinear = borderBlackLinearMipLinear;
	borderWhiteLinearMipLinear.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	borderWhiteLinearMipLinear.ShaderRegister = shaderRegisterCounter++;

	samplersDesc.push_back(borderWhiteLinearMipLinear);
}

void GlobalSamplers::Destroy()
{
}




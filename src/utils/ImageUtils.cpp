#include "ImageUtils.h"
#include "d3dx12.h"

ImageResource ImageUtils::CreateColorAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight, bool in16BitFloat)
{
	ImageResource image;
	image.resourceDescription = CD3DX12_RESOURCE_DESC::Tex2D(
		in16BitFloat ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM,
		inWidth,
		inHeight,
		1, 1, 1, 0, // array, mips and sampling
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, 
		D3D12_TEXTURE_LAYOUT_UNKNOWN);
	image.Create(inDevice, D3D12_RESOURCE_STATE_RENDER_TARGET);

	return image;
}

ImageResource ImageUtils::CreateDepthAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight)
{
	ImageResource image;
	image.resourceDescription = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		inWidth,
		inHeight,
		1, 1, 1, 0, // array, mips and sampling
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		D3D12_TEXTURE_LAYOUT_UNKNOWN);
	image.Create(inDevice, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	return image;
}


#include "ResourceView.h"
#include "core/Engine.h"
#include "../Renderer.h"

ResourceView::ResourceView()
{

}

ResourceView::ResourceView(uint16_t inBlockOffset, DescriptorBlock inDescriptorBlock)
	: blockOffset(inBlockOffset)
	, descriptorBlock(inDescriptorBlock)
{

}

ResourceView::~ResourceView()
{

}

ResourceView ResourceView::CreateSRVTexture2D(ID3D12Resource* inResource, D3D12_TEX2D_SRV inSRV, DescriptorBlock& inBlock, uint16_t inIndex)
{
	Device& device = Engine::GetRendererInstance()->GetDevice();

	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = inResource->GetDesc().Format;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D = inSRV;

	device.GetNativeDevice()->CreateShaderResourceView(inResource, &viewDesc, inBlock.GetCpuHandle(inIndex));

	return ResourceView(inIndex, inBlock);
}

ResourceView ResourceView::CreateSRVTexture2D(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex)
{
	// whole resource
	D3D12_TEX2D_SRV defaultSRV;
	defaultSRV.MipLevels = -1;
	defaultSRV.MostDetailedMip = 0;
	defaultSRV.PlaneSlice = 0;
	defaultSRV.ResourceMinLODClamp = 0.0f;

	return CreateSRVTexture2D(inResource, defaultSRV, inBlock, inIndex);
}

ResourceView ResourceView::CreateRTVTexture2D(ID3D12Resource* inResource, D3D12_TEX2D_RTV inRTV, DescriptorBlock& inBlock, uint16_t inIndex)
{
	Device& device = Engine::GetRendererInstance()->GetDevice();

	D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
	viewDesc.Format = inResource->GetDesc().Format;
	viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D = inRTV;

	device.GetNativeDevice()->CreateRenderTargetView(inResource, &viewDesc, inBlock.GetCpuHandle(inIndex));

	return ResourceView(inIndex, inBlock);
}

ResourceView ResourceView::CreateRTVTexture2D(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex)
{
	// whole resource
	D3D12_TEX2D_RTV defaultRTV;
	defaultRTV.MipSlice = 0;
	defaultRTV.PlaneSlice = 0;

	return CreateRTVTexture2D(inResource, defaultRTV, inBlock, inIndex);
}

ResourceView ResourceView::CreateDSVTexture2D(ID3D12Resource* inResource, D3D12_TEX2D_DSV inDSV, DescriptorBlock& inBlock, uint16_t inIndex)
{
	Device& device = Engine::GetRendererInstance()->GetDevice();

	D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
	viewDesc.Format = inResource->GetDesc().Format;
	viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D = inDSV;

	device.GetNativeDevice()->CreateDepthStencilView(inResource, &viewDesc, inBlock.GetCpuHandle(inIndex));

	return ResourceView(inIndex, inBlock);
}

ResourceView ResourceView::CreateDSVTexture2D(ID3D12Resource* inResource, DescriptorBlock& inBlock, uint16_t inIndex)
{
	// whole resource
	D3D12_TEX2D_DSV defaultDSV;
	defaultDSV.MipSlice = 0;

	return CreateDSVTexture2D(inResource, defaultDSV, inBlock, inIndex);
}


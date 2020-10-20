#include "Texture2D.h"
#include "core/Engine.h"
#include "render/Renderer.h"
#include "d3dx12.h"

Texture2D::Texture2D(const HashString& inPath, bool inUsesAlpha /*= false*/, bool inFlipVertical /*= true*/, bool inLinear /*= true*/, bool inGenMips /*= true*/)
	: TextureData(inPath, inUsesAlpha, inFlipVertical, inLinear, inGenMips)
{

}

Texture2D::~Texture2D()
{

}

D3D12_RESOURCE_DESC Texture2D::GetImageDesc()
{
	D3D12_RESOURCE_DESC desc;
	desc = CD3DX12_RESOURCE_DESC::Tex2D(linear ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, width, height);
	desc.MipLevels = genMips ? 0 : 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	return desc;
}
//
//ImageView Texture2D::CreateImageView(ImageSubresourceRange range)
//{
//	return image.CreateView(range, ImageViewType::e2D);
//}


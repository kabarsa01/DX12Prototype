#pragma once

#include <d3d12.h>

#include "TextureData.h"
#include <memory>

class Texture2D : public TextureData
{
public:
	Texture2D(const HashString& inPath, bool inUsesAlpha = false, bool inFlipVertical = true, bool inLinear = true, bool inGenMips = true);
	virtual ~Texture2D();
protected:
	D3D12_RESOURCE_DESC GetImageDesc() override;
//	ImageView CreateImageView(ImageSubresourceRange range) override;
private:
	Texture2D();
};

typedef std::shared_ptr<Texture2D> Texture2DPtr;


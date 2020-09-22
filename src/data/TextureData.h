#pragma once

#include "data/Resource.h"
#include "render/resources/ImageResource.h"
#include <string>

class TextureData : public Resource
{
public:
	TextureData(const HashString& inPath, bool inUsesAlpha = false, bool inFlipVertical = true, bool inLinear = true, bool inGenMips = true);
	virtual ~TextureData();

	virtual bool Load() override;
	virtual bool Cleanup() override;

	void CreateFromExternal(const ImageResource& inImage, bool inCleanup = false);

	inline ImageResource& GetImage() { return image; }
//	ImageView& GetImageView();
protected:
	ImageResource image;
//	ImageView imageView;

	std::string path;
	bool useAlpha;
	bool flipVertical;
	bool linear;
	bool genMips;
	bool cleanup;

	int width;
	int height;
	int numChannels;

	virtual D3D12_RESOURCE_DESC GetImageDesc() = 0;
//	virtual ImageView CreateImageView(ImageSubresourceRange range) = 0;
private:
	TextureData();
};

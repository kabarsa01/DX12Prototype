#pragma once

#include "render/resources/ImageResource.h"

class Device;

class ImageUtils
{
public:
	static ImageResource CreateColorAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight, bool in16BitFloat = false);
	static ImageResource CreateDepthAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight);

	static ImageResource CreateTexture2D(Device* inDevice, uint32_t inWidth, uint32_t inHeight, bool in16BitFloat = false);
	static ImageResource CreateTexture3D(Device* inDevice, uint32_t inWidth, uint32_t inHeight, uint32_t inDepth, bool in16BitFloat = false);
};

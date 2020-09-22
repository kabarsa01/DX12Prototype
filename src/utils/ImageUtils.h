#pragma once

#include "render/resources/ImageResource.h"

class Device;

class ImageUtils
{
public:
	static ImageResource CreateColorAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight, bool in16BitFloat = false);
	static ImageResource CreateDepthAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight);
};

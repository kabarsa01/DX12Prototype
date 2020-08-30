#pragma once

#include "render/resources/VulkanImage.h"

class Device;

class ImageUtils
{
public:
	static VulkanImage CreateColorAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight, bool in16BitFloat = false);
	static VulkanImage CreateDepthAttachment(Device* inDevice, uint32_t inWidth, uint32_t inHeight);
};

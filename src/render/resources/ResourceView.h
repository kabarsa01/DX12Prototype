#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "../objects/DescriptorPool.h"

class ResourceView
{
public:
	ResourceView();
	~ResourceView();
private:
	uint32_t blockOffset;
	DescriptorBlock descriptorBlock;
};
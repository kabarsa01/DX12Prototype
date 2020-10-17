#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <iostream>

#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "core/Engine.h"

int main() {
	HMODULE d3d12Module;
	d3d12Module = LoadLibraryW(L"d3d12.dll");
	if (NULL == d3d12Module)
	{
		throw std::exception("failed to load d3d12.dll");
	}

	HMODULE d3d12SDKLayersModule;
	d3d12SDKLayersModule = LoadLibraryW(L"D3d12SDKLayers.dll");
	if (NULL == d3d12SDKLayersModule)
	{
		FreeLibrary(d3d12Module);
		throw std::exception("failed to load D3d12SDKLayers.dll");
	}

	Engine* Engine = Engine::GetInstance();
	Engine->Run();

	FreeLibrary(d3d12Module);
	FreeLibrary(d3d12SDKLayersModule);

	return 0;
}
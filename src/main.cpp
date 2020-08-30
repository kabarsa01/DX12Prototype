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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "core/Engine.h"

int main() {
	Engine* Engine = Engine::GetInstance();
	Engine->Run();

	return 0;
}
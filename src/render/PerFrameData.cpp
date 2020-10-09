#include "PerFrameData.h"
#include "objects/Device.h"
#include "GlobalSamplers.h"
#include "scene/Scene.h"
#include "core/Engine.h"
#include "scene/camera/CameraComponent.h"
#include "core/TimeManager.h"
#include "scene/SceneObjectComponent.h"
#include "scene/Transform.h"
#include "scene/SceneObjectBase.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "Renderer.h"
#include "resources/ResourceView.h"

PerFrameData::PerFrameData()
{
}

PerFrameData::~PerFrameData()
{
}

void PerFrameData::Create(Device* inDevice)
{
	device = inDevice;

	globalShaderData = new GlobalShaderData();
	globalTransformData = new GlobalTransformData();

	shaderDataBuffer.Create(device, sizeof(GlobalShaderData), D3D12_HEAP_TYPE_DEFAULT);
	shaderDataBuffer.CreateStagingBuffer();
	transformDataBuffer.Create(device, sizeof(GlobalTransformData), D3D12_HEAP_TYPE_DEFAULT);
	transformDataBuffer.CreateStagingBuffer();

	D3D12_DESCRIPTOR_RANGE1 range;
	range.RegisterSpace = 0;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 2;
	range.OffsetInDescriptorsFromTableStart = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorTable.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_ALL);

	descriptorBlock = Engine::GetRendererInstance()->GetDescriptorHeaps().AllocateDescriptorsCBV_SRV_UAV(2);
	// don't even need to store these views
	ResourceView::CreateUAV(shaderDataBuffer, descriptorBlock, 0);
	ResourceView::CreateUAV(transformDataBuffer, descriptorBlock, 1);
}

void PerFrameData::Destroy()
{
	delete globalShaderData;
	delete globalTransformData;

	shaderDataBuffer.Destroy();
	transformDataBuffer.Destroy();

	DescriptorHeaps::ReleaseDescriptors(descriptorBlock);
}

void PerFrameData::UpdateBufferData()
{
	GatherData();
	shaderDataBuffer.CopyTo(sizeof(GlobalShaderData), reinterpret_cast<const char*>( globalShaderData ), true);
	transformDataBuffer.CopyTo(sizeof(GlobalTransformData), reinterpret_cast<const char*>( globalTransformData ), true);
}

void PerFrameData::GatherData()
{
	globalShaderData->time = TimeManager::GetInstance()->GetTime();
	globalShaderData->deltaTime = TimeManager::GetInstance()->GetDeltaTime();

	ScenePtr scene = Engine::GetSceneInstance();
	CameraComponentPtr camComp = scene->GetSceneComponent<CameraComponent>();

	globalShaderData->worldToView = camComp->CalculateViewMatrix();
	globalShaderData->viewToProj = camComp->CalculateProjectionMatrix();
	globalShaderData->cameraPos = camComp->GetParent()->transform.GetLocation();
	globalShaderData->viewVector = camComp->GetParent()->transform.GetForwardVector();
	globalShaderData->cameraNear = camComp->GetNearPlane();
	globalShaderData->cameraFar = camComp->GetFarPlane();
	globalShaderData->cameraFov = camComp->GetFov();
	globalShaderData->cameraAspect = camComp->GetAspectRatio();

	std::memcpy(globalTransformData->modelToWorld, scene->GetModelMatrices().data(), scene->GetRelevantMatricesCount() * sizeof(glm::mat4x4));
}


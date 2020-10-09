#pragma once

#include "scene/SceneObjectComponent.h"

#include <vector>
#include "data/MeshData.h"

class SkyboxComponent : public SceneObjectComponent
{
public:
	MeshDataPtr MeshData;
	MaterialPtr Material;

	SkyboxComponent(std::shared_ptr<SceneObjectBase> Parent);
	virtual ~SkyboxComponent();

	void SetMeshData(MeshDataPtr InMeshData);
	virtual void OnInitialize() override;
protected:
};

typedef std::shared_ptr<SkyboxComponent> SkyboxComponentPtr;
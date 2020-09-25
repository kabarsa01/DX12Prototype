#pragma once

#include <d3d12.h>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data/Resource.h"
#include "render/resources/BufferResource.h"
#include "render/resources/DeviceMemory.h"
#include "core/Engine.h"
#include "render/Renderer.h"

using namespace std;
using namespace glm;

//packing should probably be considered, maybe in the future someday
//#pragma pack(push, 1)
//#pragma pack(pop)

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;

	static std::array<D3D12_INPUT_ELEMENT_DESC, 5> GetAttributeDescriptions(uint32_t inInputSlot = 0);
};


class MeshData : public Resource
{
public:
	vector<Vertex> vertices;
	vector<uint32_t> indices;

	MeshData(const HashString& InId);
	MeshData(const HashString& InId, const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices);
	virtual ~MeshData();

	virtual void OnDestroy() override;

	// Inherited via Resource
	virtual bool Load() override;
	virtual bool Cleanup() override;

	void CreateBuffers();
	void DestroyBuffers();

	BufferResource& GetVertexBuffer();
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	uint32_t GetVertexBufferSizeBytes();
	uint32_t GetVertexCount();
	BufferResource& GetIndexBuffer();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
	uint32_t GetIndexBufferSizeBytes();
	uint32_t GetIndexCount();

//	static VertexInputBindingDescription GetBindingDescription(uint32_t inDesiredBinding);
	// fullscreen quad instance to be used for screen space stuff
	static std::shared_ptr<MeshData> FullscreenQuad();
private:
	BufferResource vertexBuffer;
	BufferResource indexBuffer;

	MeshData() : Resource(HashString::NONE) {}

	template<class T>
	void SetupBuffer(BufferResource& inBuffer, std::vector<T>& inDataVector);
};

typedef std::shared_ptr<MeshData> MeshDataPtr;

//--------------------------------------------------------------------------------------------------------------------------
// template defs
//--------------------------------------------------------------------------------------------------------------------------

template<class T>
void MeshData::SetupBuffer(BufferResource& inBuffer, std::vector<T>& inDataVector)
{
	uint64_t size = static_cast<uint64_t>(sizeof(T) * inDataVector.size());

	inBuffer.Create(&Engine::GetRendererInstance()->GetDevice(), size, D3D12_HEAP_TYPE_DEFAULT);
	inBuffer.CreateStagingBuffer();
	inBuffer.CopyTo(size, reinterpret_cast<char*>( inDataVector.data() ), true);
}




#include "data/MeshData.h"
#include "data/DataManager.h"
#include "core/Engine.h"

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

namespace
{
	std::vector<Vertex> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions            // normals           // texCoords
	{{-1.0f,  1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 1.0f,  1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
	};

	std::vector<unsigned int> quadIndices = {
		0, 1, 2,
		0, 2, 3
	};

	std::string fullscreenQuadId = "MeshData_FullscreenQuad";
};

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
//////////////////
//vec3 Position;
//vec3 Normal;
//vec2 TexCoord;
//vec3 Tangent;
//vec3 Bitangent;
/////////////////
std::array<D3D12_INPUT_ELEMENT_DESC, 5> Vertex::GetAttributeDescriptions(uint32_t inInputSlot /*= 0*/)
{
	D3D12_INPUT_ELEMENT_DESC position = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_ELEMENT_DESC normal = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, offsetof(Vertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_ELEMENT_DESC texcoord = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, inInputSlot, offsetof(Vertex, texCoord), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_ELEMENT_DESC tangent = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, offsetof(Vertex, tangent), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_ELEMENT_DESC bitangent = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, offsetof(Vertex, bitangent), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	std::array<D3D12_INPUT_ELEMENT_DESC, 5> attributes =
	{
		position,
		normal,
		texcoord,
		tangent,
		bitangent
	};

	return attributes;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

MeshData::MeshData(const HashString& inId)
	: Resource{inId}
{

}

MeshData::MeshData(const HashString& inId, const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices)
	: Resource{ inId }
	, vertices( inVertices )
	, indices( inIndices )
{
}

MeshData::~MeshData()
{
}

void MeshData::OnDestroy()
{
	DestroyBuffers();
}

void MeshData::CreateBuffers()
{
	SetupBuffer<Vertex>(vertexBuffer, vertices);
	SetupBuffer<uint32_t>(indexBuffer, indices);

	vertexBufferView = {};
	vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer.GetSizeBytes());
	vertexBufferView.BufferLocation = vertexBuffer.GetGpuVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);

	indexBufferView = {};
	indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer.GetSizeBytes());
	indexBufferView.BufferLocation = indexBuffer.GetGpuVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void MeshData::DestroyBuffers()
{
	// buffers
	vertexBuffer.Destroy();
	indexBuffer.Destroy();
}

BufferResource& MeshData::GetVertexBuffer()
{
	return vertexBuffer;
}

uint32_t MeshData::GetVertexBufferSizeBytes()
{
	return static_cast<uint32_t>( sizeof(Vertex) * vertices.size() );
}

uint32_t MeshData::GetVertexCount()
{
	return static_cast<uint32_t>( vertices.size() );
}

BufferResource& MeshData::GetIndexBuffer()
{
	return indexBuffer;
}

uint32_t MeshData::GetIndexBufferSizeBytes()
{
	return static_cast<uint32_t>(sizeof(uint32_t) * indices.size());
}

uint32_t MeshData::GetIndexCount()
{
	return static_cast<uint32_t>(indices.size());
}

MeshDataPtr MeshData::FullscreenQuad()
{
	return DataManager::GetInstance()->RequestResourceByType<MeshData, const std::vector<Vertex>&, const std::vector<unsigned int>&>(
		fullscreenQuadId,
		quadVertices,
		quadIndices
	);
}

bool MeshData::Load()
{
	return true;
}

bool MeshData::Cleanup()
{
	DestroyBuffers();
	return true;
}



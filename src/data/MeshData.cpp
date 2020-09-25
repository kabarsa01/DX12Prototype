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
	{{-1.0f,  1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{-1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
	{{ 1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
	{{ 1.0f,  1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}
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
	std::array<D3D12_INPUT_ELEMENT_DESC, 5> attributes =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, inInputSlot, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, inInputSlot, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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
}

void MeshData::DestroyBuffers()
{
	// buffers
	vertexBuffer.Destroy();
	indexBuffer.Destroy();
}

//VertexInputBindingDescription MeshData::GetBindingDescription(uint32_t inDesiredBinding)
//{
//	VertexInputBindingDescription bindingDescription;
//
//	bindingDescription.setBinding(inDesiredBinding);
//	bindingDescription.setStride(sizeof(Vertex));
//	bindingDescription.setInputRate(VertexInputRate::eVertex);
//
//	return bindingDescription;
//}

BufferResource& MeshData::GetVertexBuffer()
{
	return vertexBuffer;
}

D3D12_VERTEX_BUFFER_VIEW MeshData::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW view;
	view.SizeInBytes = vertexBuffer.GetSize();
	view.BufferLocation = vertexBuffer.GetGpuVirtualAddress();
	view.StrideInBytes = sizeof(Vertex);

	return view;
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

D3D12_INDEX_BUFFER_VIEW MeshData::GetIndexBufferView()
{
	D3D12_INDEX_BUFFER_VIEW view;
	view.SizeInBytes = indexBuffer.GetSize();
	view.BufferLocation = indexBuffer.GetGpuVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;

	return view;
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



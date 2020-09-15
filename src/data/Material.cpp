#include "Material.h"
#include "DataManager.h"
#include "core/Engine.h"
#include "render/Renderer.h"
#include "render/TransferList.h"

Material::Material(HashString inId)
	: Resource(inId)
	, vertexEntrypoint("main")
	, fragmentEntrypoint("main")
	, computeEntrypoint("main")
{

}

Material::Material(HashString inId, const std::string& inVertexShaderPath, const std::string& inFragmentShaderPath)
	: Resource(inId)
	, vertexShaderPath(inVertexShaderPath)
	, fragmentShaderPath(inFragmentShaderPath)
	, vertexEntrypoint("main")
	, fragmentEntrypoint("main")
	, computeEntrypoint("main")
{

}

Material::~Material()
{

}

void Material::LoadResources()
{
	// TODO: process all the material resources
	descriptorRanges.clear();

	vertexShader = InitShader(vertexShaderPath);
	fragmentShader = InitShader(fragmentShaderPath);
	computeShader = InitShader(computeShaderPath);

	// all the bindings are united for now
	// we have a very primitive system of assigning a range per binding, which is not beautiful but will do for now
	// TODO: group bindings by type, space and consecutive indices
	std::vector<D3D12_SHADER_INPUT_BIND_DESC> bindings;
	bindings.reserve(vertexShader->GetBindingsCount() + fragmentShader->GetBindingsCount() + computeShader->GetBindingsCount());
	bindings.insert(bindings.end(), vertexShader->GetBindings().begin(), vertexShader->GetBindings().end());
	bindings.insert(bindings.end(), fragmentShader->GetBindings().begin(), fragmentShader->GetBindings().end());
	bindings.insert(bindings.end(), computeShader->GetBindings().begin(), computeShader->GetBindings().end());

	for (uint32_t index = 0; index < bindings.size(); index++)
	{
		D3D12_SHADER_INPUT_BIND_DESC& binding = bindings[index];

		// zero space is reserved for per frame data, it's simpler to divide for now
		// TODO: manage spaces
		if (binding.Space == 0)
		{
			continue;
		}

		CD3DX12_DESCRIPTOR_RANGE1 range;
		D3D12_DESCRIPTOR_RANGE_TYPE type;

		switch (binding.Type)
		{
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
			type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			break;
		default:
			throw std::exception("Error! Shader binding type is not supported. Implement or edit shader.");
			break;
		}

		range.Init(type, 1, binding.BindPoint, binding.Space);
		descriptorRanges.push_back(range);
		nameToRange[binding.Name] = descriptorRanges.size() - 1;
	}
	descriptorTable.InitAsDescriptorTable(descriptorRanges.size(), descriptorRanges.data(), D3D12_SHADER_VISIBILITY_ALL);
	descriptorBlock = Engine::GetRendererInstance()->GetDescriptorHeaps().AllocateDescriptorsCBV_SRV_UAV(descriptorRanges.size());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//Device->GetNativeDevice()->CreateShaderResourceView();

	shaderHash = HashString(vertexShaderPath + fragmentShaderPath + computeShaderPath);
}

HashString Material::GetShaderHash()
{
	return shaderHash;
}

void Material::SetEntrypoints(const std::string& inVertexEntrypoint, const std::string& inFragmentEntrypoint)
{
	vertexEntrypoint = inVertexEntrypoint;
	fragmentEntrypoint = inFragmentEntrypoint;
}

void Material::SetVertexEntrypoint(const std::string& inEntrypoint)
{
	vertexEntrypoint = inEntrypoint;
}

void Material::SetFragmentEntrypoint(const std::string& inEntrypoint)
{
	fragmentEntrypoint = inEntrypoint;
}

void Material::SetComputeEntrypoint(const std::string& inEntrypoint)
{
	computeEntrypoint = inEntrypoint;
}

void Material::SetShaderPath(const std::string& inVertexShaderPath, const std::string& inFragmentShaderPath)
{
	vertexShaderPath = inVertexShaderPath;
	fragmentShaderPath = inFragmentShaderPath;
}

void Material::SetComputeShaderPath(const std::string& inComputeShaderPath)
{
	computeShaderPath = inComputeShaderPath;
}

void Material::SetTexture(const std::string& inName, Texture2DPtr inTexture2D)
{
	sampledImages2D[inName] = inTexture2D;
}

void Material::SetStorageTexture(const std::string& inName, Texture2DPtr inTexture2D)
{
	storageImages2D[inName] = inTexture2D;
}

void Material::SetUniformBuffer(const std::string& inName, uint64_t inSize, const char* inData)
{
	Device& device = Engine::GetRendererInstance()->GetDevice();

	BufferResource buffer;
	buffer.Create(&device);
//	buffer.BindMemory(MemoryPropertyFlagBits::eDeviceLocal);
	buffer.CreateStagingBuffer();

	buffers[inName].Destroy();
	buffers[inName] = buffer;

	UpdateUniformBuffer(inName, inSize, inData);
}

void Material::SetUniformBufferExternal(const std::string& inName, const BufferResource& inBuffer)
{
	buffers[inName].Destroy();
	buffers[inName] = inBuffer;
	buffers[inName].SetCleanup(false);
}

void Material::SetStorageBufferExternal(const std::string& inName, const BufferResource& inBuffer)
{
	storageBuffers[inName].Destroy();
	storageBuffers[inName] = inBuffer;
	storageBuffers[inName].SetCleanup(false);
}

void Material::SetStorageBuffer(const std::string& inName, uint64_t inSize, const char* inData)
{
	Device& device = Engine::GetRendererInstance()->GetDevice();

	BufferResource buffer(false);
	buffer.Create(&device);
//	buffer.BindMemory(MemoryPropertyFlagBits::eDeviceLocal);
	buffer.CreateStagingBuffer();

	storageBuffers[inName].Destroy();
	storageBuffers[inName] = buffer;
}

void Material::UpdateUniformBuffer(const std::string& inName, uint64_t inSize, const char* inData)
{
	buffers[inName].CopyTo(inSize, inData);
	TransferList::GetInstance()->PushBuffer(&buffers[inName]);
}

void Material::UpdateStorageBuffer(const std::string& inName, uint64_t inSize, const char* inData)
{
	storageBuffers[inName].CopyTo(inSize, inData);
	TransferList::GetInstance()->PushBuffer(&storageBuffers[inName]);
}

BufferResource& Material::GetUniformBuffer(const std::string& inName)
{
	return buffers[inName];
}

BufferResource& Material::GetStorageBuffer(const std::string& inName)
{
	return storageBuffers[inName];
}

bool Material::Load()
{
	return true;
}

bool Material::Cleanup()
{
	// cleanup buffers. textures are resources themselves and will be cleaned by data manager
	for (auto& pair : buffers)
	{
		pair.second.Destroy();
	}
	for (auto& pair : storageBuffers)
	{
		pair.second.Destroy();
	}
	vulkanDescriptorSet.Destroy();
	return true;
}

ShaderPtr Material::InitShader(const std::string& inResourcePath)
{
	if (!inResourcePath.empty())
	{
		ShaderPtr shader = DataManager::RequestResourceType<Shader>(inResourcePath);
		return shader;
	}
	return ShaderPtr();
}





#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"

#include "data/Resource.h"
#include "data/Texture2D.h"
#include "render/shader/Shader.h"
#include "render/objects/VulkanDescriptorSet.h"

class Device;

class Material : public Resource
{
public:
	Material(HashString inId);
	Material(HashString inId, const std::string& inVertexShaderPath, const std::string& inFragmentShaderPath);
	virtual ~Material();

	void LoadResources();
	HashString GetShaderHash();

//	void CreateDescriptorSet(Device* inDevice);

	ShaderPtr GetVertexShader();
	ShaderPtr GetFragmentShader();
	ShaderPtr GetComputeShader();

	void SetEntrypoints(const std::string& inVertexEntrypoint, const std::string& inFragmentEntrypoint);
	void SetVertexEntrypoint(const std::string& inEntrypoint);
	void SetFragmentEntrypoint(const std::string& inEntrypoint);
	void SetComputeEntrypoint(const std::string& inEntrypoint);
	void SetShaderPath(const std::string& inVertexShaderPath, const std::string& inFragmentShaderPath);
	void SetComputeShaderPath(const std::string& inComputeShaderPath);

	void SetTexture(const std::string& inName, Texture2DPtr inTexture2D);
	void SetStorageTexture(const std::string& inName, Texture2DPtr inTexture2D);
	template<typename T>
	void SetUniformBuffer(const std::string& inName, T& inUniformBuffer);
	template<typename T>
	void SetStorageBuffer(const std::string& inName, T& inStorageBuffer);
	void SetUniformBuffer(const std::string& inName, uint64_t inSize, const char* inData);
	void SetStorageBuffer(const std::string& inName, uint64_t inSize, const char* inData);
	void SetUniformBufferExternal(const std::string& inName, const BufferResource& inBuffer);
	void SetStorageBufferExternal(const std::string& inName, const BufferResource& inBuffer);
	template<typename T>
	void UpdateUniformBuffer(const std::string& inName, T& inUniformBuffer);
	void UpdateUniformBuffer(const std::string& inName, uint64_t inSize, const char* inData);
	void UpdateStorageBuffer(const std::string& inName, uint64_t inSize, const char* inData);

	BufferResource& GetUniformBuffer(const std::string& inName);
	BufferResource& GetStorageBuffer(const std::string& inName);

	inline ShaderPtr GetVertexShader() { return vertexShader; }
	inline ShaderPtr GetFragmentShader() { return fragmentShader; }
	inline ShaderPtr GetComputeShader() { return computeShader; }
	inline const std::string& GetVertexEntrypoint() const { return vertexEntrypoint; };
	inline const std::string& GetFragmentEntrypoint() const { return fragmentEntrypoint; };
	inline const std::string& GetComputeEntrypoint() const { return computeEntrypoint; };

	inline CD3DX12_ROOT_PARAMETER1& GetRootParameter() { return descriptorTable; }

	bool Load() override;
	bool Cleanup() override;
protected:
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string computeShaderPath;
	std::string vertexEntrypoint;
	std::string fragmentEntrypoint;
	std::string computeEntrypoint;
	HashString shaderHash;

	ShaderPtr vertexShader;
	ShaderPtr fragmentShader;
	ShaderPtr computeShader;
	std::map<HashString, Texture2DPtr> sampledImages2D;
	std::map<HashString, Texture2DPtr> storageImages2D;
	std::map<HashString, BufferResource> buffers;
	std::map<HashString, BufferResource> storageBuffers;

	std::vector<CD3DX12_DESCRIPTOR_RANGE1> descriptorRanges;
	std::map<HashString, uint32_t> nameToRange;
	CD3DX12_ROOT_PARAMETER1 descriptorTable;
	DescriptorBlock descriptorBlock;

	Device* vulkanDevice;
	VulkanDescriptorSet vulkanDescriptorSet;

	ShaderPtr InitShader(const std::string& inResourcePath);
	
};

typedef std::shared_ptr<Material> MaterialPtr;

//======================================================================================================================================================
// DEFINITIONS
//======================================================================================================================================================

template<typename T>
void Material::SetUniformBuffer(const std::string& inName, T& inUniformBuffer)
{
	SetUniformBuffer(inName, sizeof(T), reinterpret_cast<const char*>(&inUniformBuffer));
}

template<typename T>
void Material::SetStorageBuffer(const std::string& inName, T& inStorageBuffer)
{
	SetStorageBuffer(inName, sizeof(T), reinterpret_cast<const char*>(&inStorageBuffer));
}

template<typename T>
void Material::UpdateUniformBuffer(const std::string& inName, T& inUniformBuffer)
{
	UpdateUniformBuffer(inName, sizeof(T), reinterpret_cast<const char*>(&inUniformBuffer));
}




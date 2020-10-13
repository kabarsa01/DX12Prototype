#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "core\ObjectBase.h"
#include <memory>
#include <string>
#include <vector>
#include "data\Resource.h"
#include <d3d12shader.h>

using namespace Microsoft::WRL;

//struct BindingInfo
//{
//	uint32_t set;
//	uint32_t binding;
//	uint32_t vectorSize;
//	uint32_t numColumns;
//	HashString name;
//	HashString blockName;
//	std::vector<uint32_t> arrayDimensions;
//
//	bool IsArray()
//	{
//		return arrayDimensions.size() > 0;
//	}
//};

class Shader : public Resource
{
public:
	Shader(const HashString& inPath, const std::string& inEntryPoint, const std::string& inProfile);
	virtual ~Shader();

	virtual bool Load() override;
	virtual bool Cleanup() override;

	inline void SetEntryPoint(const std::string& inEntryPoint) { entryPoint = inEntryPoint; }

	D3D12_SHADER_BYTECODE GetBytecode();
	D3D12_SHADER_INPUT_BIND_DESC GetBindingDesc(const std::string& inName);
	inline ComPtr<ID3DBlob> GetShaderBlob() { return shaderBlob; }
	inline std::vector<D3D12_SHADER_INPUT_BIND_DESC>& GetBindings() { return bindings; }
	inline uint32_t GetBindingsCount() { return bindingsCount; }
protected:
	std::string filePath;
	std::string profile;
	std::string entryPoint;
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3D12ShaderReflection> reflection;
	std::vector<D3D12_SHADER_INPUT_BIND_DESC> bindings;
	uint32_t bindingsCount;
};

typedef std::shared_ptr<Shader> ShaderPtr;

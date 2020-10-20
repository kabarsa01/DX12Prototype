#include "render/shader/Shader.h"
#include <fstream>
#include <streambuf>
#include "core/Engine.h"
#include "../Renderer.h"
#include <d3d12shader.h>
#include <D3Dcompiler.h>
#include "utils/HelperUtils.h"

Shader::Shader(const HashString& inPath, const std::string& inEntryPoint, const std::string& inProfile)
	: Resource(inPath.GetString() + inEntryPoint + inProfile)
	, filePath(inPath.GetString())
	, entryPoint(inEntryPoint)
	, profile(inProfile)
	, bindingsCount(0)
{
}

Shader::~Shader()
{
}

bool Shader::Load()
{
	ComPtr<ID3DBlob> errorBlob;

	UINT flags = 0;// D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ALL_RESOURCES_BOUND;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG_NAME_FOR_SOURCE;
#endif
	std::wstring widecharPath = ToWString(filePath);
	ThrowIfFailed( D3DCompileFromFile(
		widecharPath.c_str(), 
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		entryPoint.c_str(), 
		profile.c_str(), 
		flags, 
		0, 
		&shaderBlob, 
		&errorBlob) );

	ThrowIfFailed( D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_PPV_ARGS(&reflection)) );
	D3D12_SHADER_DESC desc;
	ThrowIfFailed( reflection->GetDesc(&desc) );
	bindings.resize(desc.BoundResources);
	for (uint32_t index = 0; index < desc.BoundResources; index++)
	{
		D3D12_SHADER_INPUT_BIND_DESC bindingDesc;
		ThrowIfFailed( reflection->GetResourceBindingDesc(index, &bindingDesc) );
		bindings[index] = bindingDesc;
	}
	bindingsCount = static_cast<uint32_t>( bindings.size() );

	return true;
}

bool Shader::Cleanup()
{
	return true;
}

D3D12_SHADER_BYTECODE Shader::GetBytecode()
{
	D3D12_SHADER_BYTECODE bytecode;
	bytecode.BytecodeLength = shaderBlob->GetBufferSize();
	bytecode.pShaderBytecode = shaderBlob->GetBufferPointer();
	return bytecode;
}

D3D12_SHADER_INPUT_BIND_DESC Shader::GetBindingDesc(const std::string& inName)
{
	D3D12_SHADER_INPUT_BIND_DESC bindingDesc = {};
	reflection->GetResourceBindingDescByName(inName.c_str(), &bindingDesc);
	return bindingDesc;
}





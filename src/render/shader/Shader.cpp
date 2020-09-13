#include "render/shader/Shader.h"
#include <fstream>
#include <streambuf>
#include "core/Engine.h"
#include "../Renderer.h"
#include <d3d12shader.h>
#include <D3Dcompiler.h>
#include "utils/HelperUtils.h"

Shader::Shader(const HashString& inPath)
	: Resource(inPath)
{
	filePath = inPath.GetString();
}

Shader::~Shader()
{
}

bool Shader::Load()
{
	ComPtr<ID3DBlob> errorBlob;

	D3D_SHADER_MACRO defines[] { "DUMMY", "0" };
	LPCSTR target = "cs_5_0";
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	ThrowIfFailed( D3DCompileFromFile(
		ToWString(filePath).c_str(), 
		defines, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		"main", 
		target, 
		flags, 
		0, 
		&shaderBlob, 
		&errorBlob) );

	ComPtr<ID3D12ShaderReflection> reflection;
	ThrowIfFailed( D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_PPV_ARGS(&reflection)) );
	D3D12_SHADER_DESC desc;
	ThrowIfFailed( reflection->GetDesc(&desc) );
	bindings.resize(desc.BoundResources);
	for (uint32_t index; index < desc.BoundResources; index++)
	{
		D3D12_SHADER_INPUT_BIND_DESC bindingDesc;
		ThrowIfFailed( reflection->GetResourceBindingDesc(index, &bindingDesc) );
		bindings[index] = bindingDesc;
	}
	bindingsCount = bindings.size();

	return true;
}

bool Shader::Cleanup()
{
	return true;
}







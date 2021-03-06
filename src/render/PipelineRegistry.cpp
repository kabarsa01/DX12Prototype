#include "PipelineRegistry.h"

PipelineRegistry* PipelineRegistry::instance = new PipelineRegistry();

PipelineRegistry::PipelineRegistry()
{
}

PipelineRegistry::PipelineRegistry(const PipelineRegistry& inOther)
{
}

PipelineRegistry::~PipelineRegistry()
{
}

void PipelineRegistry::operator=(const PipelineRegistry& inOther)
{
}

PipelineRegistry* PipelineRegistry::GetInstance()
{
	return instance;
}

void PipelineRegistry::DestroyPipelines(Device* inDevice)
{
	//ComPtr<ID3D12Device2> device = inDevice->GetNativeDevice();
	//for (auto& passPair : pipelinesData)
	//{
	//	for (auto& shaderPair : passPair.second)
	//	{
	//		device.destroyPipelineLayout(shaderPair.second.pipelineLayout);
	//		device.destroyPipeline(shaderPair.second.pipeline);
	//	}
	//}
	pipelinesData.clear();
}

void PipelineRegistry::DestroyPipelines(Device* inDevice, HashString inPassHash)
{
	//Device& device = inDevice->GetNativeDevice();
	if (pipelinesData.find(inPassHash) != pipelinesData.end())
	{
		/*for (auto& shaderPair : pipelinesData[inPassHash])
		{
			device.destroyPipelineLayout(shaderPair.second.pipelineLayout);
			device.destroyPipeline(shaderPair.second.pipeline);
		}*/
		pipelinesData[inPassHash].clear();
	}
}

bool PipelineRegistry::HasPipeline(HashString inPassHash, HashString inShadersHash)
{
	if (pipelinesData.find(inPassHash) != pipelinesData.end())
	{
		std::map<HashString, PipelineData>& passData = pipelinesData[inPassHash];
		return passData.find(inShadersHash) != passData.end();
	}
	return false;
}

bool PipelineRegistry::StorePipeline(HashString inPassHash, HashString inShadersHash, PipelineData inPipelineData)
{
	pipelinesData[inPassHash][inShadersHash] = inPipelineData;
	return true;
}

PipelineData& PipelineRegistry::GetPipeline(HashString inPassHash, HashString inShadersHash)
{
	return pipelinesData[inPassHash][inShadersHash];
}

std::map<HashString, PipelineData>& PipelineRegistry::operator[](HashString inShaderHash)
{
	return pipelinesData[inShaderHash];
}


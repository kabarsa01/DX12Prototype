#pragma once

#include <wrl.h>
#include <d3d12.h>

#include <map>
#include "common/HashString.h"
#include "objects/Device.h"
#include "objects/VulkanDescriptorSet.h"

using namespace Microsoft::WRL;

struct PipelineData
{
	ComPtr<ID3D12PipelineState> pipeline;
	ComPtr<ID3D12RootSignature> rootSignature;
//	PipelineLayout pipelineLayout;
//	std::vector<DescriptorSet> descriptorSets;
};

class PipelineRegistry
{
public:
	static PipelineRegistry* GetInstance();

	void DestroyPipelines(Device* inDevice);
	void DestroyPipelines(Device* inDevice, HashString inPassHash);

	bool HasPipeline(HashString inPassHash, HashString inShadersHash);
	bool StorePipeline(HashString inPassHash, HashString inShadersHash, PipelineData inPipelineData);
	PipelineData& GetPipeline(HashString inPassHash, HashString inShadersHash);

	std::map<HashString, PipelineData>& operator[](HashString inPassHash);
private:
	static PipelineRegistry* instance;

	//std::map<HashString, Pipeline> pipelines;
	// render pass hash - shader hash - pipeline data
	std::map<HashString, std::map<HashString, PipelineData>> pipelinesData;

	PipelineRegistry();
	PipelineRegistry(const PipelineRegistry& inOther);
	virtual ~PipelineRegistry();
	void operator=(const PipelineRegistry& inOther);
};
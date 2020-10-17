#ifndef _DEFAULT_INCLUDES_HLSL
#define _DEFAULT_INCLUDES_HLSL

struct VSInput
{
	uint instanceId : SV_InstanceID;

	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

cbuffer RootConstants : register(b0, space0)
{
	uint transformIndexOffset;
}

SamplerState repeatLinearSampler : register(s0, space0);
SamplerState repeatMirrorLinearSampler : register(s1, space0);
SamplerState borderBlackLinearSampler : register(s2, space0);
SamplerState borderWhiteLinearSampler : register(s3, space0);

cbuffer ShaderGlobalData : register(b1, space0)
{
	float4x4 worldToView;
	float4x4 viewToProj;
	float3 cameraPos;
	float3 viewVector;
	float time;
	float deltaTime;
	float cameraNear;
	float cameraFar;
	float cameraFov;
	float cameraAspect;
};

RWStructuredBuffer<float4x4> globalTransformData : register(u0, space0);

#endif //_DEFAULT_INCLUDES_HLSL

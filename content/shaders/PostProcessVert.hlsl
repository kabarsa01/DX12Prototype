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

struct GlobalTransformData
{
	float4x4 modelToWorld[1024];
};
RWStructuredBuffer<float4x4> globalTransformData : register(u0, space0);

struct VSInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
};

Texture2D<float4> screenImage : register(t0, space1);

PSInput VSmain(VSInput input)
{
	PSInput psIn;

    psIn.pos = float4(input.pos, 1.0);
	psIn.color = frac(time);//input.normal*0.5 + 0.5;//inPos + vec3(0.5, 0.5, 0.5);
	psIn.uv = input.uv;

	return psIn;
}

float4 PSmain(PSInput input) : SV_TARGET
{
	float4 texColor = screenImage.Sample(repeatLinearSampler, input.uv);
	return float4(input.color * texColor, 1.0f);
}
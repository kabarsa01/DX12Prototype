#include "./DefaultIncludes.hlsl"

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
	float4 texColor = screenImage.SampleLevel(repeatLinearSampler, input.uv, 0);
	return texColor;//float4(0.7, 0.7, 0.2, 1.0);//
}
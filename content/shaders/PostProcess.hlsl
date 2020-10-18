#include "./DefaultIncludes.hlsl"

static const float gammaPower = 1.0 / 2.2;

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
	psIn.color = frac(time);
	psIn.uv = input.uv;

	return psIn;
}

float4 PSmain(PSInput input) : SV_TARGET
{
	float4 texColor = screenImage.SampleLevel(repeatLinearSampler, input.uv, 0);
    texColor.xyz = texColor.xyz / (texColor.xyz + float3(1.0, 1.0, 1.0));
    return pow(texColor, float4(gammaPower, gammaPower, gammaPower, 1.0));
}
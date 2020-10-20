#include "./DefaultIncludes.hlsl"
#include "./Matrix.hlsl"

struct PSInput
{
	float4 pos : SV_Position;
	float3 worldPos : POSITION;
	float3 worldNormal : NORMAL;
	float2 uv : TEXCOORD;
	float3x3 TBN : TBN_MATRIX;
};

struct PSOut
{
	float4 albedo : SV_Target0;
	float4 normal : SV_Target1;
	float4 packedRM : SV_Target2;
};

Texture2D<float4> albedo : register(t0, space1);
Texture2D<float4> normal : register(t1, space1);

//================================================================================================
// entrypoints
//================================================================================================

PSInput VSmain(VSInput input)
{
	PSInput vsOut;

	float4x4 modelMatrix = globalTransformData[transformIndexOffset + input.instanceId];

	vsOut.worldPos = mul(modelMatrix, float4(input.pos, 1.0)).xyz;
	vsOut.uv = input.uv;
	float3x3 normalTransformMatrix = transpose(inverse((float3x3)modelMatrix));
	vsOut.worldNormal = normalize( mul(normalTransformMatrix, input.normal) );

	vsOut.pos = mul( viewToProj, mul( worldToView, mul(modelMatrix, float4(input.pos, 1.0)) ) );

	float3 worldTangent = mul(modelMatrix, float4(input.tangent, 0.0f)).xyz;
	float3 worldBitangent = mul(modelMatrix, float4(input.bitangent, 0.0f)).xyz;
    float3 worldNormal = mul(modelMatrix, float4(input.normal, 0.0f)).xyz;
    vsOut.TBN = transpose(float3x3(worldTangent, worldBitangent, worldNormal));

	return vsOut;
}

PSOut PSmain(PSInput input)
{
	PSOut output;

    output.albedo = albedo.SampleLevel(repeatLinearSampler, input.uv, 0);
    float3 tangentNormal = normal.SampleLevel(repeatLinearSampler, input.uv, 0).xyz;
	tangentNormal = tangentNormal * 2.0 - 1.0;
    output.normal = float4(normalize(mul(input.TBN, tangentNormal)), 1.0);

	return output;
}



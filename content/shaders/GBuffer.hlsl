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

//float3 PerturbNormal(float2 uv)
//{
//	vec3 pos_dx = dFdx(fragInput.worldPos);
//	vec3 pos_dy = dFdy(fragInput.worldPos);
//    vec3 tex_dx = dFdx(vec3(fragInput.uv, 0.0));
//    vec3 tex_dy = dFdy(vec3(fragInput.uv, 0.0));
//    vec3 t = (tex_dy.y * pos_dx - tex_dx.y * pos_dy) / (tex_dx.x * tex_dy.y - tex_dy.x * tex_dx.y);

//    vec3 ng = normalize(fragInput.worldNormal);

//    t = normalize(t - ng * dot(ng, t));
//    vec3 b = normalize(cross(ng, t));
//    mat3 tbn = mat3(t, b, ng);
    
//    return normalize(tbn * tangentNormal);
//}

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

	float3 worldTangent = mul(modelMatrix, float4(input.tangent, 1.0f)).xyz;
	float3 worldBitangent = mul(modelMatrix, float4(input.bitangent, 1.0f)).xyz;
	float3 worldNormal = mul(modelMatrix, float4(input.normal, 1.0f)).xyz;
	vsOut.TBN = float3x3(worldTangent, worldBitangent, worldNormal);

	return vsOut;
}

PSOut PSmain(PSInput input)
{
	PSOut output;

    output.albedo = albedo.SampleLevel(repeatLinearSampler, input.uv, 0);
	float3 tangentNormal = normal.SampleLevel(repeatLinearSampler, input.uv, 0).xyz * 2.0 - 1.0;
	output.normal = float4(mul(input.TBN, tangentNormal), 1.0);

	return output;
}



#include "./DefaultIncludes.hlsl"
#include "./Matrix.hlsl"

// Pi =)
static const float PI = 3.14159265359;

// light clustering data
RWByteAddressBuffer lightClusters : register(u0, space1);
RWByteAddressBuffer lightClustersIndices : register(u1, space1);

// cluster lights
struct LightInfo
{
    float4 position;
    float4 direction;
    float4 color;
    float4 rai;
};

cbuffer lightsList : register(b0, space1)
{
    LightInfo lights[1024];
};

cbuffer lightsIndices : register(b1, space1)
{
    uint2 directionalPosition;
    uint2 spotPosition;
    uint2 pointPosition;
};

Texture2D<float4> albedoTex : register(t0, space1);
Texture2D<float4> normalsTex : register(t1, space1);
Texture2D<float4> roghnessTex : register(t2, space1);
Texture2D<float4> metallnessTex : register(t3, space1);
Texture2D<float4> depthTex : register(t4, space1);
Texture2D<float4> debugClustersTexture : register(t5, space1);

uint UnpackLightIndex(uint packedIndices, uint indexPosition)
{
	uint bitOffset = 16 * (indexPosition % 2);
	return (packedIndices >> bitOffset) & 0x0000ffff;
}

float3 CalculateSpec(float3 inViewDir, float3 inLightDir, float3 inNormal, float3 inSpecColor, float inSpecStrength)
{
    // blinn-phong intermediate floattor and spec value calculation
	float3 intermediate = normalize(inViewDir + inLightDir);
	return inSpecColor * pow(max(dot(intermediate, inNormal), 0.0), 32) * inSpecStrength;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    float gloss = 1.0 - roughness;
    return F0 + (max(float3(gloss, gloss, gloss), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 CalculateLightInfluence(float3 albedo, float3 N, float3 V, float3 F, float3 inPixelToLightDir, float3 inLightColor, float3 kD, float roughness)
{
		float3 L = normalize(inPixelToLightDir);
        float3 H = normalize(V + L);

        float attenuation = 1.0;
        float3 radiance = inLightColor * attenuation;// * (1.0 - shadowAttenuation);

        float NDF = DistributionGGX(N, H, roughness);  
        float G = GeometrySmith(N, V, L, roughness);

        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        float3 specular = numerator / max(denominator, 0.001);
  
        float NdotL = max(dot(N, L), 0.0);        
        return (kD * albedo / PI + specular) * radiance * NdotL;
}

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

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
    uint clusterX = uint(clamp(input.uv.x * 32.0, 0, 31));
    uint clusterY = uint(clamp(input.uv.y * 32.0, 0, 31));

	float near = cameraNear;
	float far = cameraFar;
    float depth = depthTex.SampleLevel(repeatLinearSampler, input.uv, 0).r;
	if (depth == 0.0 || depth >= 1.0)
	{
        return float4(0.0, 0.0, 0.0, 1.0);
    }

	//float linearDepth = 2.0 * near * far / (far + depth * (near - far));
    
    float4 projPos = float4((-1.0f + 2.0 * input.uv.x), (1.0f - 2.0 * input.uv.y), depth, 1.0);
    float4 viewPos = mul(inverse(viewToProj), projPos);
    viewPos /= viewPos.w;
    float4 pixelCoordWorld = mul(inverse(worldToView), viewPos);
    uint clusterIndex = clamp(uint(64.0 * log(abs(viewPos.z) / near) / log(far / near)), 0, 63); // clump it just in case

	// byte addresses for byte address buffers
    uint clusterIndicesByteAddress = ((clusterX * 32 + clusterY) * 64 + clusterIndex) * 128 * 4; //allways 4 bytes aligned
    uint clusterByteAddress = ((clusterX * 32 + clusterY) * 64 + clusterIndex) * 8; //uint2 is 8 bytes
	
    uint2 lightsPositions = lightClusters.Load2(clusterByteAddress);
	uint directionalCount = (lightsPositions.x >> 8) & 0x000000ff;
	uint directionalOffset = lightsPositions.x & 0x000000ff;
	uint spotCount = (lightsPositions.x >> 24) & 0x000000ff;
	uint spotOffset = (lightsPositions.x >> 16) & 0x000000ff;
	uint pointCount = (lightsPositions.y >> 8) & 0x000000ff;
	uint pointOffset = lightsPositions.y & 0x000000ff;

	float3 albedo = albedoTex.SampleLevel(repeatLinearSampler, input.uv, 0).xyz;
	float3 N = normalsTex.SampleLevel(repeatLinearSampler, input.uv, 0).xyz;
	float roughness = 1.0;//texture( sampler2D( roughnessTex, repeatLinearSampler ), uv ).r;
	float metallness = 0.0;//texture( sampler2D( metallnessTex, repeatLinearSampler ), uv ).r;

	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	float3 V = normalize(cameraPos - pixelCoordWorld.xyz);
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallness);
    float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallness;
	// some ambient lighting
    float3 irradiance = float3(0.03f, 0.03f, 0.03f); //texture(irradianceMap, N).rgb;
    float3 diffuse = irradiance * albedo;
    float3 ambient = (kD * diffuse);// * AO;

    for (uint dirIndex = directionalOffset; dirIndex < directionalOffset + directionalCount; dirIndex++)
	{
        uint lightIndicesPacked = lightClustersIndices.Load(clusterIndicesByteAddress + (dirIndex * 4 / 2));
        LightInfo lightInfo = lights[UnpackLightIndex(lightIndicesPacked, dirIndex)];

		float3 pixelToLightDir = -lightInfo.direction.xyz;
		float surfaceCosine = max(dot(normalize(pixelToLightDir), N), 0.0);

		Lo += CalculateLightInfluence(albedo, N, V, F, pixelToLightDir, lightInfo.color.xyz * lightInfo.rai.z, kD, roughness);
	}
    for (uint spotIndex = spotOffset; spotIndex < spotOffset + spotCount; spotIndex++)
	{
        uint lightIndicesPacked = lightClustersIndices.Load(clusterIndicesByteAddress + (spotIndex * 4 / 2));
        LightInfo lightInfo = lights[UnpackLightIndex(lightIndicesPacked, spotIndex)];

		float3 pixelToLightDir = (lightInfo.position - pixelCoordWorld).xyz;

		float cosine = dot( normalize(-1.0 * pixelToLightDir), normalize(lightInfo.direction.xyz) );
		float angleFactor = clamp(cosine - cos(radians(lightInfo.rai.y)), 0, 1);
		float lightRadiusSqr = lightInfo.rai.x * lightInfo.rai.x;
		float pixelDistanceSqr = dot(pixelToLightDir, pixelToLightDir);
		float distanceFactor = max(lightRadiusSqr - pixelDistanceSqr, 0) / max(lightRadiusSqr, 0.001);

		float3 lightColor = lightInfo.color.xyz * lightInfo.rai.z * angleFactor * distanceFactor;

		Lo += CalculateLightInfluence(albedo, N, V, F, pixelToLightDir, lightColor, kD, roughness);
	}
    for (uint pointIndex = pointOffset; pointIndex < pointOffset + pointCount; pointIndex++)
	{
        uint lightIndicesPacked = lightClustersIndices.Load(clusterIndicesByteAddress + (pointIndex * 4 / 2));
        LightInfo lightInfo = lights[UnpackLightIndex(lightIndicesPacked, pointIndex)];

		float3 pixelToLightDir = (lightInfo.position - pixelCoordWorld).xyz;
		float lightRadiusSqr = lightInfo.rai.x * lightInfo.rai.x;
		float pixelDistanceSqr = dot(pixelToLightDir, pixelToLightDir);
		float distanceFactor = max(lightRadiusSqr - pixelDistanceSqr, 0) / lightRadiusSqr;
		float surfaceCosine = max(dot(normalize(pixelToLightDir), N), 0.0);

		float3 lightColor = lightInfo.color.xyz * lightInfo.rai.z * distanceFactor;
		Lo += CalculateLightInfluence(albedo, N, V, F, pixelToLightDir, lightColor, kD, roughness);
	}

	return float4(ambient + Lo, 1.0);

}

#include "./DefaultIncludes.hlsl"
#include "./Matrix.hlsl"

Texture2D<float4> depthTexture : register(t0, space1);

RWByteAddressBuffer lightClusters : register(u0, space1);
RWByteAddressBuffer lightClustersIndices : register(u1, space1);
RWTexture2D<float4> debugClustersTexture : register(u2, space1);

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

// shared
groupshared uint clusterStatus[64];

uint Pack16BitIndex(uint lightIndexData, uint lightIndexInCluster, uint lightIndexGlobal)
{
	uint bitOffset = 16 * (lightIndexInCluster % 2);
	// nullify
	lightIndexData &= 0xffff0000 >> bitOffset;
	// set
	lightIndexData |= lightIndexGlobal << bitOffset;

	return lightIndexData;
}

bool IsConeIntersectsSphere(float3 coneOrigin, float3 coneDir, float coneLength, float halfAngle, float4 sphere)
{
    const float3 centerToCenter = sphere.xyz - coneOrigin;
    const float centersDistanceSqr = dot(centerToCenter, centerToCenter);
	// vector between cone and sphere centers projected onto cone direction, do not forget to normalize coneDir
    const float projectionOnCone = dot(centerToCenter, coneDir);
	// this is simply can be calculated looking at similar triangles, v2 * tan(angle) - v1 * sin(angle) gives us distance to cone border,
	// where v2 is a distance from sphere center to the end of projection vector, which is vector between cone and sphere centers projected on cone direction
    const float distanceToConeBorder = cos(halfAngle) * sqrt(centersDistanceSqr - projectionOnCone*projectionOnCone) - projectionOnCone * sin(halfAngle);
 
    const bool isSphereOutsideConeAngle = distanceToConeBorder > sphere.w;
    const bool isOutsideFront = projectionOnCone > (sphere.w + coneLength);
    const bool isBehindBack  = projectionOnCone < -sphere.w;

	return !(isSphereOutsideConeAngle || isOutsideFront || isBehindBack);
}

struct CSInput
{
    uint3 groupId : SV_GroupID;
    uint3 threadId : SV_GroupThreadID;
    uint threadIndex : SV_GroupIndex;
};

[numthreads(8, 8, 1)]
void main(CSInput input)
{
	if (input.threadIndex == 0)
	{
		for (uint idx = 0; idx < 64; idx++)
		{
			clusterStatus[idx] = 0;
		}
	}
    GroupMemoryBarrierWithGroupSync();

	float near = cameraNear;
	float far = cameraFar;

    uint2 screenRes;
    depthTexture.GetDimensions(screenRes.x, screenRes.y);
	float2 pixelUVHalfSize =  0.5 / float2(screenRes);
	float2 tileSize = float2(screenRes) / float2(32.0f,32.0f);

	uint2 tileStart = uint2(float2(0.5, 0.5) + tileSize * float2(input.groupId.xy) );
    uint2 tileFinish = uint2(float2(0.5, 0.5) + tileSize * (float2(input.groupId.xy + uint2(1,1))));
    float2 uvTileStart = float2(tileStart) / float2(screenRes);
    float2 uvTileFinish = float2(tileFinish) / float2(screenRes);

	uint2 tilePixelSize = tileFinish - tileStart;
	uint tilePixelsCount = tilePixelSize.x * tilePixelSize.y;
	if (input.threadIndex >= tilePixelsCount)
	{
		return;
	}

	uint workgroupSize = 8 * 8 * 1;
    uint pixelCount = (tilePixelsCount / workgroupSize) + clamp((tilePixelsCount % workgroupSize) / input.threadIndex, 0, 1);

	for (uint index = 0; index < pixelCount; index++)
	{
        uint pixelIndex = input.threadIndex + (index * workgroupSize);
		uint2 tileCoord = uint2(pixelIndex % tilePixelSize.x, pixelIndex / tilePixelSize.x);
		float2 pixelUV = pixelUVHalfSize + float2(tileStart + tileCoord) / float2(screenRes);

        float depth = depthTexture.SampleLevel(repeatLinearSampler, pixelUV, 0).r;
		if (depth <= 0.0 || depth >= 1.0)
		{
			continue;
		}
		
        float4 projPos = float4((-1.0f + 2.0 * pixelUV.x), (1.0f - 2.0 * pixelUV.y), depth, 1.0);
        float4 viewPos = mul(inverse(viewToProj), projPos);
        viewPos /= viewPos.w;

        //float linearDepth = near * far / (far + depth * (near - far));
		// depthSlice = near * (far/near) ^ (slice/numSlices) --- pretty good distribution from Doom 2016
		// cluster index = maxClusters * log(linearDepth/near) / log(far/near);
        uint clusterIndex = clamp(uint(64.0 * log(abs(viewPos.z / near)) / log(far / near)), 0, 63); // clump it for zero based index just in case
		uint nextClusterIndex = clusterIndex + 1;
		
        uint clusterChecked;
        InterlockedOr(clusterStatus[clusterIndex], 1, clusterChecked);
		if (clusterChecked != 0)
		{
			continue;
		}

		// TODO: find the light source intersections for this cluster
		// first we find our cluster bounds for AABB. clusters are small so it's an 'okay' approximation
		// view space is constructed with negative Z looking in camera lens direction, so depth
		// grows in negative direction, -1.0 multiplier is used
        float clusterNear = -1.0 * near * pow(abs(far / near), float(clusterIndex) / 64.0);
        float clusterFar = -1.0 * near * pow(abs(far / near), float(nextClusterIndex) / 64.0);

		// while still using glm::perspective remember that FOV is vertical
		float fovMultiplier = tan(radians(cameraFov * 0.5)) * 2.0;
        float nearHeight = abs(clusterNear * fovMultiplier);
        float farHeight = abs(clusterFar * fovMultiplier);
        float aspectRatio = float(screenRes.x) / float(screenRes.y);
        float nearWidth = nearHeight * aspectRatio;
        float farWidth = farHeight * aspectRatio;

        float clusterLeft = min(nearWidth * (-0.5 + input.groupId.x / 32.0), farWidth * (-0.5 + input.groupId.x / 32.0));
        float clusterRight = max(nearWidth * (-0.5 + (input.groupId.x + 1) / 32.0), farWidth * (-0.5 + (input.groupId.x + 1) / 32.0));

        float clusterTop = max(nearHeight * (0.5 - input.groupId.y / 32.0), farHeight * (0.5 - input.groupId.y / 32.0));
        float clusterBottom = min(nearHeight * (0.5 - (input.groupId.y + 1) / 32.0), farHeight * (0.5 - (input.groupId.y + 1) / 32.0));

		// we make an AABB from our cluster frustum and then we make a sphere out of it
		float4 boundingSphere;
		float3 clusterExtentMax = float3(clusterRight, clusterTop, clusterNear);
		float3 clusterExtentMin = float3(clusterLeft, clusterBottom, clusterFar);
		float3 extents = (clusterExtentMax - clusterExtentMin) * 0.5;
		boundingSphere.xyz = clusterExtentMin + extents;
		boundingSphere.w = length(extents);

		uint lightIndexInCluster = 0;
        uint clusterIndicesByteAddress = ((input.groupId.x * 32 + input.groupId.y) * 64 + clusterIndex) * 128 * 4; //allways 4 bytes aligned
        uint clusterByteAddress = ((input.groupId.x * 32 + input.groupId.y) * 64 + clusterIndex) * 8; //uint2 is 8 bytes
        uint2 clusterLightsOffsetData = lightClusters.Load2(clusterByteAddress);

        for (uint dirIndex = directionalPosition.x; dirIndex < directionalPosition.x + directionalPosition.y; dirIndex++)
		{
            LightInfo lightInfo = lights[dirIndex];

			uint entryIndex = lightIndexInCluster / 2;
            uint lightIndexData = lightClustersIndices.Load(clusterIndicesByteAddress + entryIndex * 4);
            lightIndexData = Pack16BitIndex(lightIndexData, lightIndexInCluster, dirIndex);
            lightClustersIndices.Store(clusterIndicesByteAddress + entryIndex * 4, lightIndexData);

			lightIndexInCluster++;
		}
        uint directionalLightOffsetData = clusterLightsOffsetData.x;
		directionalLightOffsetData &= 0xffff0000;
		// offset is always 0
		directionalLightOffsetData |= 0x0000ff00 & (lightIndexInCluster << 8);
        clusterLightsOffsetData.x = directionalLightOffsetData;

		uint spotLightOffset = lightIndexInCluster;
		uint spotLightCount = 0;
        for (uint spotIndex = spotPosition.x; spotIndex < spotPosition.x + spotPosition.y; spotIndex++)
		{
			if (lightIndexInCluster > 255)
			{
				break;
			}

            LightInfo lightInfo = lights[spotIndex];
            float4 lightViewPos = mul(worldToView, float4(lightInfo.position.xyz, 1.0));
            lightViewPos /= lightViewPos.w;
            float3 lightPos = lightViewPos.xyz;
            float3 lightDir = normalize(mul(worldToView, float4(lightInfo.direction.xyz, 0.0)).xyz);

			if (!IsConeIntersectsSphere(lightPos, lightDir, lightInfo.rai.x, radians(lightInfo.rai.y), boundingSphere))
			{
				continue;
			}
			spotLightCount++;

			uint entryIndex = lightIndexInCluster / 2;
            uint lightIndexData = lightClustersIndices.Load(clusterIndicesByteAddress + entryIndex * 4);
            lightIndexData = Pack16BitIndex(lightIndexData, lightIndexInCluster, spotIndex);
            lightClustersIndices.Store(clusterIndicesByteAddress + entryIndex * 4, lightIndexData);

			lightIndexInCluster++;
		}
        uint spotLightOffsetData = clusterLightsOffsetData.x;
		spotLightOffsetData &= 0x0000ffff;
		spotLightOffsetData |= 0x00ff0000 & (spotLightOffset << 16);
		spotLightOffsetData |= 0xff000000 & (spotLightCount << 24);
        clusterLightsOffsetData.x = spotLightOffsetData;

		uint pointLightOffset = lightIndexInCluster;
		uint pointLightCount = 0;
        for (uint pointIndex = pointPosition.x; pointIndex < pointPosition.x + pointPosition.y; pointIndex++)
		{
			if (lightIndexInCluster > 255)
			{
				break;
			}
            LightInfo lightInfo = lights[pointIndex];
            float4 lightViewPos = mul(worldToView, float4(lightInfo.position.xyz, 1.0));
            lightViewPos /= lightViewPos.w;
            float3 lightPos = lightViewPos.xyz;

			float3 lightSphereClippedPos = clamp(lightPos, clusterExtentMin, clusterExtentMax);
			float3 centerToAABB = lightSphereClippedPos - lightPos;

			if (dot(centerToAABB, centerToAABB) > (lightInfo.rai.x * lightInfo.rai.x))
			{
				continue;
			}
			pointLightCount++;

			uint entryIndex = lightIndexInCluster / 2;
            uint lightIndexData = lightClustersIndices.Load(clusterIndicesByteAddress + entryIndex * 4);
            lightIndexData = Pack16BitIndex(lightIndexData, lightIndexInCluster, pointIndex);
            lightClustersIndices.Store(clusterIndicesByteAddress + entryIndex * 4, lightIndexData);

			lightIndexInCluster++;
		}
        uint pointLightOffsetData = clusterLightsOffsetData.y;
		pointLightOffsetData &= 0xffff0000;
		pointLightOffsetData |= 0x000000ff & pointLightOffset;
		pointLightOffsetData |= 0x0000ff00 & (pointLightCount << 8);
        clusterLightsOffsetData.y = pointLightOffsetData;

        lightClusters.Store2(clusterByteAddress, clusterLightsOffsetData);
		
        debugClustersTexture[tileStart + tileCoord] = float4(clusterLeft, clusterTop, clusterFar, 1.0);
    }
}

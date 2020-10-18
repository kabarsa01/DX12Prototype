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

cbuffer LightsList : register(b0, space1)
{
    LightInfo lights[1024];
};

cbuffer LightsIndices : register(b1, space1)
{
    uint2 directionalPosition;
    uint2 spotPosition;
    uint2 pointPosition;
};
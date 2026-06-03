
struct IndirectCommand
{
    uint mInstanceIndex;
    
    uint IndexCountPerInstance;
    uint InstanceCount;
    uint StartIndexLocation;
    int BaseVertexLocation;
    uint StartInstanceLocation;
};

AppendStructuredBuffer<IndirectCommand> gCommands : register(u0);

struct Params
{
    uint gInstanceCount;
    uint gIndexCount;
};

ConstantBuffer<Params> gParams : register(b0);

cbuffer Camera : register(b1)
{
    float32_t4 gFrustumPlanes[6];
    // (x,y,z,w) = ax + by + cz + d
    
    float4x4 ViewProj;
    float2 ScreenSize;
    uint MipCount;
};

struct InstanceData
{
    float32_t3 min;
    float32_t3 max;
};

StructuredBuffer<InstanceData> gInstanceData : register(t0);



Texture2D<float> HiZ : register(t1);
SamplerState PointClamp : register(s0);

void GetAABBCorners(float32_t3 min, float32_t3 max, out float3 c[8])
{
    c[0] = float3(min.x, min.y, min.z);
    c[1] = float3(max.x, min.y, min.z);
    c[2] = float3(min.x, max.y, min.z);
    c[3] = float3(max.x, max.y, min.z);
    c[4] = float3(min.x, min.y, max.z);
    c[5] = float3(max.x, min.y, max.z);
    c[6] = float3(min.x, max.y, max.z);
    c[7] = float3(max.x, max.y, max.z);
}

bool ProjectAABB(
    float32_t3 aabbmin,
    float32_t3 aabbmax,
    out float2 uvMin,
    out float2 uvMax,
    out float nearestDepth)
{
    float3 corners[8];
    GetAABBCorners(aabbmin, aabbmax, corners);

    uvMin = float2(1e9, 1e9);
    uvMax = float2(-1e9, -1e9);
    nearestDepth = 1.0;

    bool anyValid = false;

    [unroll]
    for (int i = 0; i < 8; i++)
    {
        float4 clip = mul(float4(corners[i], 1), ViewProj);

        if (clip.w <= 0)
        {
            nearestDepth = 0.0;
            anyValid = true;
            continue;
        }

        anyValid = true;

        float3 ndc = clip.xyz / clip.w;
        float2 uv = ndc.xy * float2(0.5, -0.5) + 0.5;

        uvMin = min(uvMin, uv);
        uvMax = max(uvMax, uv);
        nearestDepth = min(nearestDepth, ndc.z);
    }

    return anyValid;
}

uint SelectMip(float2 uvMin, float2 uvMax)
{
    float2 pixelSize = (uvMax - uvMin) * ScreenSize;
    float maxDim = max(pixelSize.x, pixelSize.y);
    maxDim = max(maxDim, 1.0); // 1px未満を防ぐ

    float mip = log2(maxDim) - 1.0; // 安全側バイアス
    mip = clamp(mip, 0, (float) (MipCount - 1));

    return (uint) mip;
}



bool InsideOrIntersect(float32_t3 min, float32_t3 max, float32_t4 plane)
{
    float32_t3 center = (min + max) * 0.5f;
    float32_t3 halfSize = (max - min) * 0.5f;
    float32_t d = dot(plane.xyz, center) + plane.w;
    float32_t r =
        halfSize.x * abs(plane.x) +
        halfSize.y * abs(plane.y) +
        halfSize.z * abs(plane.z);
    return d >= -r;
}

bool Intersect(float32_t3 min, float32_t3 max)
{
    for (int i = 0; i < 6; ++i)
    {
        if (!InsideOrIntersect(min, max, gFrustumPlanes[i]))
            return false;
    }
    return true;
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;
    if (i >= gParams.gInstanceCount)
        return;
    
    
    
    

    float2 uvMin, uvMax;
    float nearestDepth;

    if (!ProjectAABB(gInstanceData[i].min, gInstanceData[i].max, uvMin, uvMax, nearestDepth))
    {
        return;
    }

    uvMin = saturate(uvMin);
    uvMax = saturate(uvMax);

    // MIPレベル選択
    uint mip = SelectMip(uvMin, uvMax);

    // サンプリング点を9点に拡張（四隅＋辺中点＋中心）
    float2 samplePoints[9] =
    {
        uvMin,
        float2(uvMax.x, uvMin.y),
        float2(uvMin.x, uvMax.y),
        uvMax,
        float2((uvMin.x + uvMax.x) * 0.5, uvMin.y),
        float2((uvMin.x + uvMax.x) * 0.5, uvMax.y),
        float2(uvMin.x, (uvMin.y + uvMax.y) * 0.5),
        float2(uvMax.x, (uvMin.y + uvMax.y) * 0.5),
        float2((uvMin.x + uvMax.x) * 0.5, (uvMin.y + uvMax.y) * 0.5)
    };

    float hizDepth = 0.0;
    [unroll]
    for (int j = 0; j < 9; j++)
    {
        hizDepth = max(hizDepth, HiZ.SampleLevel(PointClamp, samplePoints[j], mip));
    }
    if (nearestDepth > hizDepth)
    {
        return;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    // ここでカリング
    if (!Intersect(gInstanceData[i].min, gInstanceData[i].max))
        return;

    IndirectCommand cmd;
    cmd.mInstanceIndex = i;
    cmd.IndexCountPerInstance = gParams.gIndexCount;
    cmd.InstanceCount = 1;
    cmd.StartIndexLocation = 0;
    cmd.BaseVertexLocation = 0;
    cmd.StartInstanceLocation = 0;

    gCommands.Append(cmd);
}

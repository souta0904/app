#include "Grass.hlsli"

struct VSInput
{
    float32_t4 pos : POSITION0;
    float32_t3 normal : NORMAL0;
    float32_t2 uv : TEXCOORD0;
};

struct TransformationMatrix
{
    float32_t4x4 mWorld;
};
StructuredBuffer<TransformationMatrix> gTransformationMatrix : register(t0);

cbuffer RootConstants : register(b0)
{
    uint gInstanceIndex;
};

struct Scene
{
    float32_t4x4 mVP;
    float32_t mTime;
};
ConstantBuffer<Scene> gScene : register(b1);

VSOutput main(VSInput input)
{
    VSOutput output;
    
    float32_t4 wpos = mul(input.pos, gTransformationMatrix[gInstanceIndex].mWorld);
    
    float32_t speed = 2.0f;
    float32_t space = 0.05f;
    float32_t scale = 0.8f;
    
    float32_t seed = gInstanceIndex * 10.0f;
    float32_t windX = sin(gScene.mTime * speed + wpos.x * space + gInstanceIndex);
    float32_t windZ = cos(gScene.mTime * speed + wpos.z * space + gInstanceIndex);
    
    float32_t factor = saturate(input.pos.y);
    wpos.xz += float32_t2(windX, windZ) * scale * factor;
    
    output.svpos = mul(wpos, gScene.mVP);
    output.normal = input.normal;
    output.uv = input.uv;

    return output;
}

#include "Model.hlsli"

struct VSInput
{
    float32_t4 pos : POSITION0;
    float32_t3 normal : NORMAL0;
    float32_t2 uv : TEXCOORD0;
};

struct TransformationMatrix
{
    float32_t4x4 mWorld;
    float32_t4x4 mWVP;
    float32_t4x4 mWorldInvTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    output.wpos = mul(input.pos, gTransformationMatrix.mWorld).xyz;
    output.svpos = mul(input.pos, gTransformationMatrix.mWVP);
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.mWorldInvTranspose));
    output.uv = input.uv;
    return output;
}

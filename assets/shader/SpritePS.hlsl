#include "Sprite.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float32_t4 color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    float32_t4 transformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gConstant.mUVTransform);
    float32_t4 texColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = texColor * gConstant.mColor;
    return output;
}

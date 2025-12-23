#include "Model.hlsli"

struct PSOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float32_t4 mColor;
};

struct Camera
{
    float32_t3 mPosition;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
#ifdef NO_TEXTURE
    output.color = gMaterial.mColor;
#else
    float32_t4 transformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gConstant.mUVTransform);
    float32_t4 texColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.mColor * texColor;
#endif
    return output;
}

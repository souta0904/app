#include "Model.hlsli"

#define MAX_DIRECTIONAL_LIGHT_COUNT 8

struct PSOutput
{
    float32_t4 color : SV_TARGET0;
};

enum class SpecularType
{
    None,
    Phong,
    BlinnPhong,
};

struct Material
{
    float32_t4 mColor;
    uint32_t mIsLighting;
    SpecularType mSpecularType;
    float32_t mShininess;
};

struct Camera
{
    float32_t3 mPosition;
};

struct DirectionalLight
{
    float32_t4 mColor;
    float32_t3 mDirection;
    float32_t mIntensity;
};

struct Light
{
    DirectionalLight mDirectionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
    uint32_t mDirectionalLightCount;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Light> gLight : register(b2);

PSOutput main(VSOutput input)
{
    PSOutput output;
    output.color = 0.0f;
    
    float32_t4 surfaceColor;
#ifdef NO_TEXTURE
    surfaceColor = gMaterial.mColor;
#else
    float32_t4 transformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gConstant.mUVTransform);
    float32_t4 texColor = gTexture.Sample(gSampler, transformedUV.xy);
    surfaceColor = gMaterial.mColor * texColor;
#endif
    
    if (!gMaterial.mIsLighting)
    {
        output.color = surfaceColor;
        return output;
    }
    
    float32_t3 N = normalize(input.normal);
    float32_t3 V = normalize(gCamera.mPosition - input.wpos);
    float32_t3 diffuse = 0.0f;
    float32_t3 specular = 0.0f;
    
    // directional light
    [loop]
    for (uint32_t i = 0; i < gLight.mDirectionalLightCount; ++i)
    {
        float32_t3 L = -gLight.mDirectionalLights[i].mDirection;
        float32_t3 lightColor = gLight.mDirectionalLights[i].mColor.rgb * gLight.mDirectionalLights[i].mIntensity;
        
        // diffuse
        float32_t NdotL = saturate(dot(N, L));
        float32_t cos = NdotL * 0.5f + 0.5f;
        diffuse += surfaceColor.rgb * lightColor * cos * cos;

        // specular
        if (gMaterial.mSpecularType != SpecularType::None)
        {
            float32_t specularPower = 0.0f;
            if (gMaterial.mSpecularType == SpecularType::Phong)
            {
                float32_t3 R = reflect(-L, N);
                specularPower = pow(saturate(dot(R, V)), gMaterial.mShininess);
            }
            else if (gMaterial.mSpecularType == SpecularType::BlinnPhong)
            {
                float32_t3 H = normalize(L + V);
                specularPower = pow(saturate(dot(N, H)), gMaterial.mShininess);
            }
            specular += float32_t3(1.0f, 1.0f, 1.0f) * lightColor * specularPower;
        }
    }
    
    output.color.rgb = diffuse + specular;
    output.color.a = surfaceColor.a;
    
    return output;
}

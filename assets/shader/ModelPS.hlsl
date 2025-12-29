#include "Model.hlsli"

#define MAX_DIRECTIONAL_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT 8

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

struct PointLight
{
    float32_t4 mColor;
    float32_t3 mPosition;
    float32_t mIntensity;
    float32_t mRadius;
    float32_t mDecay;
    
    float32_t2 mPadding;
};

struct Light
{
    DirectionalLight mDirectionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
    PointLight mPointLights[MAX_POINT_LIGHT_COUNT];
    uint32_t mDirectionalLightCount;
    uint32_t mPointLightCount;
    
    float32_t2 mPadding;
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
    float32_t3 toEye = normalize(gCamera.mPosition - input.wpos);
    float32_t3 diffuse = 0.0f;
    float32_t3 specular = 0.0f;
    
    bool isSpecular = gMaterial.mSpecularType != SpecularType::None;
    bool isBlinnPhong = gMaterial.mSpecularType == SpecularType::BlinnPhong;
    
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
        if (isSpecular)
        {
            float32_t specularPower = 0.0f;
            if (!isBlinnPhong)
            {
                float32_t3 R = reflect(-L, N);
                specularPower = pow(saturate(dot(R, toEye)), gMaterial.mShininess);
            }
            else
            {
                float32_t3 H = normalize(L + toEye);
                specularPower = pow(saturate(dot(N, H)), gMaterial.mShininess);
            }
            specular += float32_t3(1.0f, 1.0f, 1.0f) * lightColor * specularPower;
        }
    }
    
    // point light
    [loop]
    for (i = 0; i < gLight.mPointLightCount; ++i)
    {
        float32_t3 toLight = gLight.mPointLights[i].mPosition - input.wpos;
        float32_t distanceSq = dot(toLight, toLight);
        float32_t radiusSq = gLight.mPointLights[i].mRadius * gLight.mPointLights[i].mRadius;
        if (distanceSq >= radiusSq) continue;
        
        float32_t3 L = toLight * rsqrt(distanceSq);
        float32_t3 lightColor = gLight.mPointLights[i].mColor.rgb * gLight.mPointLights[i].mIntensity;
        float32_t factor = pow(saturate(1.0f - sqrt(distanceSq / radiusSq)), gLight.mPointLights[i].mDecay);
        
        // diffuse
        float32_t NdotL = saturate(dot(N, L));
        float32_t cos = NdotL * 0.5f + 0.5f;
        diffuse += surfaceColor.rgb * lightColor * cos * cos * factor;
        
        // specular
        if (isSpecular)
        {
            float32_t specularPower = 0.0f;
            if (!isBlinnPhong)
            {
                float32_t3 R = reflect(-L, N);
                specularPower = pow(saturate(dot(R, toEye)), gMaterial.mShininess);
            }
            else
            {
                float32_t3 H = normalize(L + toEye);
                specularPower = pow(saturate(dot(N, H)), gMaterial.mShininess);
            }
            specular += float32_t3(1.0f, 1.0f, 1.0f) * lightColor * specularPower * factor;
        }
    }
    
    output.color.rgb = diffuse + specular;
    output.color.a = surfaceColor.a;
    
    return output;
}

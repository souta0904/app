#include "Grass.hlsli"

struct PSOutput
{
    float32_t4 color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output;

    float h = saturate(1.0f - input.uv.x);

    // 緑寄りに調整
    float3 colorLow = float3(0.04, 0.20, 0.04); // 少しだけ暗く
    float3 colorMid = float3(0.22, 0.60, 0.08); // 緑強め
    float3 colorHigh = float3(0.65, 0.80, 0.20); // 黄色弱め

    float t1 = smoothstep(0.0, 0.5, h);
    float3 midBlend = lerp(colorLow, colorMid, t1);

    float t2 = smoothstep(0.3, 1.0, h);
    float3 baseColor = lerp(midBlend, colorHigh, t2);

    // トゥーン光（そのまま）
    float3 lightDir = normalize(float3(0.3, 1.0, 0.2));
    float NdotL = saturate(dot(input.normal, lightDir));

    float toon = saturate(NdotL * 0.6 + 0.4);

    baseColor *= toon;

    output.color = float4(baseColor, 1.0f);
    return output;
}

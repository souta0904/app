#include "Sprite.hlsli"

struct VSInput
{
    float32_t4 pos : POSITION0;
    float32_t2 uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(input.pos, gConstant.mWVP);
    output.uv = input.uv;
    return output;
}

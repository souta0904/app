#include "Primitive.hlsli"

struct VSInput
{
    float32_t4 pos : POSITION0;
    float32_t4 color : COLOR0;
};

struct Constant
{
    float32_t4x4 mVP;
};

ConstantBuffer<Constant> gConstant : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(input.pos, gConstant.mVP);
    output.color = input.color;
    return output;
}

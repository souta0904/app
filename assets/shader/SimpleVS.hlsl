#include "Simple.hlsli"

struct VSInput
{
    float32_t4 mPosition : POSITION0;
    float32_t4 mColor : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.mSVPosition = input.mPosition;
    output.mColor = input.mColor;
    return output;
}

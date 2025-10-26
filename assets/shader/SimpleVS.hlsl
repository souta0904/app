#include "Simple.hlsli"

struct VSInput
{
    float32_t4 pos : POSITION0;
    float32_t4 color : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = input.pos;
    output.color = input.color;
    return output;
}

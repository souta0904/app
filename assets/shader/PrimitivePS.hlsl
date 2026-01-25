#include "Primitive.hlsli"

struct PSOutput
{
    float32_t4 color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    output.color = input.color;
    return output;
}

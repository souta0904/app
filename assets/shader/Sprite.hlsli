
struct Constant
{
    float32_t4x4 mWVP;
    float32_t4 mColor;
};

ConstantBuffer<Constant> gConstant : register(b0);

struct VSOutput
{
    float32_t4 pos : SV_POSITION0;
    float32_t2 uv : TEXCOORD0;
};

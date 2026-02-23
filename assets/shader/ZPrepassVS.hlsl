
struct VSInput
{
    float32_t4 pos : POSITION0;
};

struct TransformationMatrix
{
    float32_t4x4 mWorld;
    float32_t4x4 mWVP;
    float32_t4x4 mWorldInvTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

float32_t4 main(VSInput input) : SV_POSITION0
{
    return mul(input.pos, gTransformationMatrix.mWVP);
}

Texture2D<float> gSrcMip : register(t0);
RWTexture2D<float> gDstMip : register(u0);

cbuffer HiZConstants : register(b0)
{
    uint2 gSrcSize;
    uint padding[2];
};

[numthreads(8, 8, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint2 dstCoord = dtid.xy;

    uint2 dstSize;
    gDstMip.GetDimensions(dstSize.x, dstSize.y);
    if (dstCoord.x >= dstSize.x || dstCoord.y >= dstSize.y)
        return;

    int2 srcCoord = int2(dstCoord) * 2;
    int2 srcMax = int2(gSrcSize) - 1;

    // 端を clamp して画面外アクセスを防ぐ
    float d0 = gSrcMip.Load(int3(clamp(srcCoord, int2(0, 0), srcMax), 0));
    float d1 = gSrcMip.Load(int3(clamp(srcCoord + int2(1, 0), int2(0, 0), srcMax), 0));
    float d2 = gSrcMip.Load(int3(clamp(srcCoord + int2(0, 1), int2(0, 0), srcMax), 0));
    float d3 = gSrcMip.Load(int3(clamp(srcCoord + int2(1, 1), int2(0, 0), srcMax), 0));

    // 通常Zの場合は「最も奥」を保持 → max
    float result = max(max(d0, d1), max(d2, d3));

    gDstMip[dstCoord] = result;
}

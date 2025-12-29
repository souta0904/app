#pragma once
#include "math/Color.h"
#include "math/Vector3.h"

/// <summary>
/// 点光源
/// </summary>
struct PointLight
{
    // 色
    Color mColor;
    // 位置
    Vector3 mPosition;
    // 明るさ
    float mIntensity;
    // 半径
    float mRadius;
    // 距離減衰の指数
    float mDecay;
};

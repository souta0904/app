#pragma once
#include "math/Color.h"
#include "math/Vector3.h"

/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight
{
    // 色
    Color mColor;
    // 向き
    Vector3 mDirection;
    // 明るさ
    float mIntensity;
};

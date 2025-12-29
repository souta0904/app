#pragma once
#include "math/Color.h"
#include "math/Vector3.h"

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight
{
    // 色
    Color mColor;
    // 向き
    Vector3 mDirection;
    // 明るさ
    float mIntensity;
    // 位置
    Vector3 mPosition;
    // 半径
    float mRadius;
    // 減衰率
    float mDecay;
    // 内角
    float mInnerAngle;
    // 外角
    float mOuterAngle;
};

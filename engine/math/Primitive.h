#pragma once
#include <limits>

#include "Vector2.h"
#include "Vector3.h"

/// <summary>
/// 2Dの直線
/// </summary>
struct Line2D
{
    static constexpr float kMinT = std::numeric_limits<float>::lowest();
    static constexpr float kMaxT = ( std::numeric_limits<float>::max )();
    Vector2 mStart;
    Vector2 mEnd;
};

/// <summary>
/// 3Dの直線
/// </summary>
struct Line3D
{
    static constexpr float kMinT = std::numeric_limits<float>::lowest();
    static constexpr float kMaxT = ( std::numeric_limits<float>::max )();
    Vector3 mStart;
    Vector3 mEnd;
};

/// <summary>
/// 2Dの半直線
/// </summary>
struct Ray2D
{
    static constexpr float kMinT = 0.0f;
    static constexpr float kMaxT = ( std::numeric_limits<float>::max )();
    Vector2 mStart;
    Vector2 mEnd;
};

/// <summary>
/// 3Dの半直線
/// </summary>
struct Ray3D
{
    static constexpr float kMinT = 0.0f;
    static constexpr float kMaxT = ( std::numeric_limits<float>::max )();
    Vector3 mStart;
    Vector3 mEnd;
};

/// <summary>
/// 2Dの線分
/// </summary>
struct Segment2D
{
    static constexpr float kMinT = 0.0f;
    static constexpr float kMaxT = 1.0f;
    Vector2 mStart;
    Vector2 mEnd;
};

/// <summary>
/// 3Dの線分
/// </summary>
struct Segment3D
{
    static constexpr float kMinT = 0.0f;
    static constexpr float kMaxT = 1.0f;
    Vector3 mStart;
    Vector3 mEnd;
};

/// <summary>
/// 2DのAABB
/// </summary>
struct AABB2D
{
    Vector2 mMin;
    Vector2 mMax;
};

/// <summary>
/// 3DのAABB
/// </summary>
struct AABB3D
{
    Vector3 mMin;
    Vector3 mMax;
};

/// <summary>
/// 2DのOBB
/// </summary>
struct OBB2D
{
    Vector2 mCenter;
    Vector2 mHalfSize;
    Vector2 mAxes[2];
};

/// <summary>
/// 3DのOBB
/// </summary>
struct OBB3D
{
    Vector3 mCenter;
    Vector3 mHalfSize;
    Vector3 mAxes[3];
};

/// <summary>
/// 円
/// </summary>
struct Circle
{
    Vector2 mCenter;
    float mRadius;
};

/// <summary>
/// 球
/// </summary>
struct Sphere
{
    Vector3 mCenter;
    float mRadius;
};

/// <summary>
/// 2Dのカプセル
/// </summary>
struct Capsule2D
{
    Segment2D mSegment;
    float mRadius;
};

/// <summary>
/// 3Dのカプセル
/// </summary>
struct Capsule3D
{
    Segment3D mSegment;
    float mRadius;
};

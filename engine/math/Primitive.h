#pragma once
#include <limits>

#include "Matrix4.h"
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

    void Reset()
    {
        mMin = Vector3( +FLT_MAX, +FLT_MAX, +FLT_MAX );
        mMax = Vector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    }

    void Update( const Vector3& p )
    {
        mMin.x = ( std::min )( mMin.x, p.x );
        mMin.y = ( std::min )( mMin.y, p.y );
        mMin.z = ( std::min )( mMin.z, p.z );
        mMax.x = ( std::max )( mMax.x, p.x );
        mMax.y = ( std::max )( mMax.y, p.y );
        mMax.z = ( std::max )( mMax.z, p.z );
    }
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

/// <summary>
/// 平面
/// </summary>
struct Plane
{
    Vector3 mNormal;
    float mD;
};

/// <summary>
/// 視錐台
/// </summary>
struct Frustum
{
    Plane mPlanes[6];

    void Build( const Matrix4& vpMat )
    {
        // 左
        mPlanes[0].mNormal.x = vpMat.m[0][3] + vpMat.m[0][0];
        mPlanes[0].mNormal.y = vpMat.m[1][3] + vpMat.m[1][0];
        mPlanes[0].mNormal.z = vpMat.m[2][3] + vpMat.m[2][0];
        mPlanes[0].mD = vpMat.m[3][3] + vpMat.m[3][0];
        // 右
        mPlanes[1].mNormal.x = vpMat.m[0][3] - vpMat.m[0][0];
        mPlanes[1].mNormal.y = vpMat.m[1][3] - vpMat.m[1][0];
        mPlanes[1].mNormal.z = vpMat.m[2][3] - vpMat.m[2][0];
        mPlanes[1].mD = vpMat.m[3][3] - vpMat.m[3][0];
        // 下
        mPlanes[2].mNormal.x = vpMat.m[0][3] + vpMat.m[0][1];
        mPlanes[2].mNormal.y = vpMat.m[1][3] + vpMat.m[1][1];
        mPlanes[2].mNormal.z = vpMat.m[2][3] + vpMat.m[2][1];
        mPlanes[2].mD = vpMat.m[3][3] + vpMat.m[3][1];
        // 上
        mPlanes[3].mNormal.x = vpMat.m[0][3] - vpMat.m[0][1];
        mPlanes[3].mNormal.y = vpMat.m[1][3] - vpMat.m[1][1];
        mPlanes[3].mNormal.z = vpMat.m[2][3] - vpMat.m[2][1];
        mPlanes[3].mD = vpMat.m[3][3] - vpMat.m[3][1];
        // near
        mPlanes[4].mNormal.x = vpMat.m[0][3] + vpMat.m[0][2];
        mPlanes[4].mNormal.y = vpMat.m[1][3] + vpMat.m[1][2];
        mPlanes[4].mNormal.z = vpMat.m[2][3] + vpMat.m[2][2];
        mPlanes[4].mD = vpMat.m[3][3] + vpMat.m[3][2];
        // far
        mPlanes[5].mNormal.x = vpMat.m[0][3] - vpMat.m[0][2];
        mPlanes[5].mNormal.y = vpMat.m[1][3] - vpMat.m[1][2];
        mPlanes[5].mNormal.z = vpMat.m[2][3] - vpMat.m[2][2];
        mPlanes[5].mD = vpMat.m[3][3] - vpMat.m[3][2];

        // 正規化
        for( auto& plane : mPlanes )
        {
            float len = Length( plane.mNormal );
            plane.mNormal /= len;
            plane.mD /= len;
        }
    }
};

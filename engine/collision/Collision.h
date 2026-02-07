#pragma once
#include "math/Primitive.h"

/// <summary>
/// AABBが平面と衝突or内側か
/// </summary>
/// <param name="aabb"></param>
/// <param name="plane"></param>
/// <returns></returns>
inline bool InsideOrIntersect( const AABB3D& aabb, const Plane& plane )
{
    auto center = ( aabb.mMin + aabb.mMax ) * 0.5f;
    auto halfSize = ( aabb.mMax - aabb.mMin ) * 0.5f;
    auto d = Dot( plane.mNormal, center ) + plane.mD;
    auto r =
        halfSize.x * std::fabs( plane.mNormal.x ) +
        halfSize.y * std::fabs( plane.mNormal.y ) +
        halfSize.z * std::fabs( plane.mNormal.z );
    return d >= -r;
}

/// <summary>
/// AABBと視錐台の衝突判定
/// </summary>
/// <param name="aabb"></param>
/// <param name="frustum"></param>
/// <returns></returns>
inline bool Intersect( const AABB3D& aabb, const Frustum& frustum )
{
    for( auto plane : frustum.mPlanes )
    {
        if( !InsideOrIntersect( aabb, plane ) ) return false;
    }
    return true;
}

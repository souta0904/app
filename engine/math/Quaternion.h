#pragma once
#include "Vector3.h"

/// <summary>
/// クォータニオン
/// </summary>
class Quaternion
{
   public:
    float w;
    float x;
    float y;
    float z;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Quaternion()
        : w( 1.0f )
        , x( 0.0f )
        , y( 0.0f )
        , z( 0.0f )
    {
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="w">W成分</param>
    /// <param name="x">X成分</param>
    /// <param name="y">Y成分</param>
    /// <param name="z">Z成分</param>
    Quaternion( float w, float x, float y, float z )
        : w( w )
        , x( x )
        , y( y )
        , z( z )
    {
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="axis">回転軸</param>
    /// <param name="rotate">回転角</param>
    Quaternion( const Vector3& axis, float rotate )
    {
        float half = rotate * 0.5f;
        float sin = std::sin( half );
        w = cosf( half );
        x = axis.x * sin;
        y = axis.y * sin;
        z = axis.z * sin;
    }

    /// <summary>
    /// 正規化
    /// </summary>
    void Normalize()
    {
        float lenSq = w * w + x * x + y * y + z * z;
        if( lenSq > MathUtil::kEpsilon )
        {
            float invLen = 1.0f / std::sqrt( lenSq );
            w *= invLen;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
    }
};

/// <summary>
/// 共役
/// </summary>
inline Quaternion Conjugate( const Quaternion& a )
{
    return Quaternion( a.w, -a.x, -a.y, -a.z );
}

/// <summary>
/// 乗算
/// </summary>
inline Quaternion operator*( const Quaternion& a, const Quaternion& b )
{
    Quaternion quat;
    quat.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    quat.x = a.w * b.x + a.x * b.w + a.z * b.y - a.y * b.z;
    quat.y = a.w * b.y + a.y * b.w + a.x * b.z - a.z * b.x;
    quat.z = a.w * b.z + a.z * b.w + a.y * b.x - a.x * b.y;
    return quat;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3 operator*( const Vector3& a, const Quaternion& b )
{
    Quaternion quat;
    Quaternion p = Quaternion( 0.0f, a.x, a.y, a.z );
    quat = b * p * Conjugate( b );
    return Vector3( quat.x, quat.y, quat.z );
}

/// <summary>
/// 乗算
/// </summary>
inline Quaternion operator*=( Quaternion& a, const Quaternion& b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3& operator*=( Vector3& a, const Quaternion& b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 内積
/// </summary>
inline float Dot( const Quaternion& a, const Quaternion& b )
{
    return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

/// <summary>
/// 線形補間
/// </summary>
inline Quaternion Lerp( const Quaternion& a, const Quaternion& b, float t )
{
    Quaternion quat;
    quat.w = a.w + ( b.w - a.w ) * t;
    quat.x = a.x + ( b.x - a.x ) * t;
    quat.y = a.y + ( b.y - a.y ) * t;
    quat.z = a.z + ( b.z - a.z ) * t;
    quat.Normalize();
    return quat;
}

/// <summary>
/// べき乗
/// </summary>
inline Quaternion Pow( const Quaternion& a, float e )
{
    float rot = std::acos( a.w );
    float sin = std::sin( rot );
    if( std::fabs( sin ) < MathUtil::kEpsilon )
    {
        return a;
    }
    Quaternion quat;
    float newRot = rot * e;
    quat.w = std::cos( newRot );
    float denom = std::sin( newRot ) / sin;
    quat.x = a.x * denom;
    quat.y = a.y * denom;
    quat.z = a.z * denom;
    return quat;
}

/// <summary>
/// 球面線型補間
/// </summary>
inline Quaternion Slerp( const Quaternion& a, const Quaternion& b, float t )
{
    if( t <= 0.0f ) return a;
    if( t >= 1.0f ) return b;

    float cos = Dot( a, b );
    float w = b.w;
    float x = b.x;
    float y = b.y;
    float z = b.z;
    if( cos < 0.0f )
    {
        w = -b.w;
        x = -b.x;
        y = -b.y;
        z = -b.z;
        cos = -cos;
    }

    float k0, k1;
    if( cos > 1.0f - MathUtil::kEpsilon )
    {
        k0 = 1.0f - t;
        k1 = t;
    }
    else
    {
        float sin = std::sqrt( 1.0f - cos * cos );
        float rot = std::atan2( sin, cos );
        float invSin = 1.0f / sin;
        k0 = std::sin( ( 1.0f - t ) * rot ) * invSin;
        k1 = std::sin( t * rot ) * invSin;
    }

    Quaternion quat;
    quat.w = k0 * a.w + k1 * w;
    quat.x = k0 * a.x + k1 * x;
    quat.y = k0 * a.y + k1 * y;
    quat.z = k0 * a.z + k1 * z;
    return quat;
}

/// <summary>
/// 回転軸を取得
/// </summary>
inline Vector3 GetAxis( const Quaternion& a )
{
    float sin = std::sqrt( 1.0f - a.w * a.w );
    if( sin < MathUtil::kEpsilon )
    {
        return Vector3::kUnitX;
    }
    float invSin = 1.0f / sin;
    return Vector3( a.x * invSin, a.y * invSin, a.z * invSin );
}

/// <summary>
/// 回転角を取得
/// </summary>
inline float GetRotate( const Quaternion& a )
{
    return std::acos( a.w ) * 2.0f;
}

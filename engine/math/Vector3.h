#pragma once
#include <cmath>

#include "MathUtil.h"

/// <summary>
/// 3次元ベクトル
/// </summary>
class Vector3
{
   public:
    static const Vector3 kZero;
    static const Vector3 kOne;
    static const Vector3 kUnitX;
    static const Vector3 kUnitY;
    static const Vector3 kUnitZ;

    float x;
    float y;
    float z;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Vector3()
        : x( 0.0f )
        , y( 0.0f )
        , z( 0.0f )
    {
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="x">X成分</param>
    /// <param name="y">Y成分</param>
    /// <param name="z">Z成分</param>
    Vector3( float x, float y, float z )
        : x( x )
        , y( y )
        , z( z )
    {
    }

    /// <summary>
    /// 正規化
    /// </summary>
    void Normalize()
    {
        float lenSq = x * x + y * y + z * z;
        if( lenSq > MathUtil::kEpsilon )
        {
            float invLen = 1.0f / std::sqrt( lenSq );
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
    }
};

/// <summary>
/// 等しいか
/// </summary>
inline bool operator==( const Vector3& a, const Vector3& b )
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/// <summary>
/// 等しくないか
/// </summary>
inline bool operator!=( const Vector3& a, const Vector3& b )
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

/// <summary>
/// 反転
/// </summary>
inline Vector3 operator-( const Vector3& a )
{
    return Vector3( -a.x, -a.y, -a.z );
}

/// <summary>
/// 加算
/// </summary>
inline Vector3 operator+( const Vector3& a, const Vector3& b )
{
    return Vector3( a.x + b.x, a.y + b.y, a.z + b.z );
}

/// <summary>
/// 減算
/// </summary>
inline Vector3 operator-( const Vector3& a, const Vector3& b )
{
    return Vector3( a.x - b.x, a.y - b.y, a.z - b.z );
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3 operator*( const Vector3& a, float b )
{
    return Vector3( a.x * b, a.y * b, a.z * b );
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3 operator*( float a, const Vector3& b )
{
    return Vector3( a * b.x, a * b.y, a * b.z );
}

/// <summary>
/// 除算
/// </summary>
inline Vector3 operator/( const Vector3& a, float b )
{
    float denom = 1.0f / b;
    return Vector3( a.x * denom, a.y * denom, a.z * denom );
}

/// <summary>
/// 加算
/// </summary>
inline Vector3& operator+=( Vector3& a, const Vector3& b )
{
    a = a + b;
    return a;
}

/// <summary>
/// 減算
/// </summary>
inline Vector3& operator-=( Vector3& a, const Vector3& b )
{
    a = a - b;
    return a;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3& operator*=( Vector3& a, float b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 除算
/// </summary>
inline Vector3& operator/=( Vector3& a, float b )
{
    float denom = 1.0f / b;
    a *= denom;
    return a;
}

/// <summary>
/// 外積
/// </summary>
inline Vector3 Cross( const Vector3& a, const Vector3& b )
{
    Vector3 vec;
    vec.x = a.y * b.z - a.z * b.y;
    vec.y = a.z * b.x - a.x * b.z;
    vec.z = a.x * b.y - a.y * b.x;
    return vec;
}

/// <summary>
/// 内積
/// </summary>
inline float Dot( const Vector3& a, const Vector3& b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/// <summary>
/// 大きさ
/// </summary>
inline float Length( const Vector3& a )
{
    return std::sqrt( a.x * a.x + a.y * a.y + a.z * a.z );
}

/// <summary>
/// 大きさの2乗
/// </summary>
inline float LengthSq( const Vector3& a )
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

/// <summary>
/// 正規化
/// </summary>
inline Vector3 Normalize( const Vector3& a )
{
    Vector3 vec = a;
    vec.Normalize();
    return vec;
}

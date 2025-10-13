#pragma once
#include <cmath>

/// <summary>
/// 4次元ベクトル
/// </summary>
class Vector4
{
   public:
    static const Vector4 kZero;
    static const Vector4 kOne;
    static const Vector4 kUnitX;
    static const Vector4 kUnitY;
    static const Vector4 kUnitZ;
    static const Vector4 kUnitW;

    float x;
    float y;
    float z;
    float w;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Vector4()
        : x( 0.0f )
        , y( 0.0f )
        , z( 0.0f )
        , w( 0.0f )
    {
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="x">X成分</param>
    /// <param name="y">Y成分</param>
    /// <param name="z">Z成分</param>
    /// <param name="w">W成分</param>
    Vector4( float x, float y, float z, float w )
        : x( x )
        , y( y )
        , z( z )
        , w( w )
    {
    }
};

/// <summary>
/// 等しいか
/// </summary>
inline bool operator==( const Vector4& a, const Vector4& b )
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/// <summary>
/// 等しくないか
/// </summary>
inline bool operator!=( const Vector4& a, const Vector4& b )
{
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

/// <summary>
/// 反転
/// </summary>
inline Vector4 operator-( const Vector4& a )
{
    return Vector4( -a.x, -a.y, -a.z, -a.w );
}

/// <summary>
/// 加算
/// </summary>
inline Vector4 operator+( const Vector4& a, const Vector4& b )
{
    return Vector4( a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w );
}

/// <summary>
/// 減算
/// </summary>
inline Vector4 operator-( const Vector4& a, const Vector4& b )
{
    return Vector4( a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w );
}

/// <summary>
/// 乗算
/// </summary>
inline Vector4 operator*( const Vector4& a, float b )
{
    return Vector4( a.x * b, a.y * b, a.z * b, a.w * b );
}

/// <summary>
/// 乗算
/// </summary>
inline Vector4 operator*( float a, const Vector4& b )
{
    return Vector4( a * b.x, a * b.y, a * b.z, a * b.w );
}

/// <summary>
/// 除算
/// </summary>
inline Vector4 operator/( const Vector4& a, float b )
{
    float denom = 1.0f / b;
    return Vector4( a.x * denom, a.y * denom, a.z * denom, a.w * denom );
}

/// <summary>
/// 加算
/// </summary>
inline Vector4& operator+=( Vector4& a, const Vector4& b )
{
    a = a + b;
    return a;
}

/// <summary>
/// 減算
/// </summary>
inline Vector4& operator-=( Vector4& a, const Vector4& b )
{
    a = a - b;
    return a;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector4& operator*=( Vector4& a, float b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 除算
/// </summary>
inline Vector4& operator/=( Vector4& a, float b )
{
    float denom = 1.0f / b;
    a *= denom;
    return a;
}

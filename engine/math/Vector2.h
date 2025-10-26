#pragma once
#include <cmath>

#include "MathUtil.h"

/// <summary>
/// 2次元ベクトル
/// </summary>
class Vector2
{
   public:
    static const Vector2 kZero;
    static const Vector2 kOne;
    static const Vector2 kUnitX;
    static const Vector2 kUnitY;

    float x;
    float y;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Vector2()
        : x( 0.0f )
        , y( 0.0f )
    {
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="x">X成分</param>
    /// <param name="y">Y成分</param>
    Vector2( float x, float y )
        : x( x )
        , y( y )
    {
    }

    /// <summary>
    /// 正規化
    /// </summary>
    void Normalize()
    {
        float lenSq = x * x + y * y;
        if( lenSq > MathUtil::kEpsilon )
        {
            float invLen = 1.0f / std::sqrt( lenSq );
            x *= invLen;
            y *= invLen;
        }
    }
};

/// <summary>
/// 等しいか
/// </summary>
inline bool operator==( const Vector2& a, const Vector2& b )
{
    return a.x == b.x && a.y == b.y;
}

/// <summary>
/// 等しくないか
/// </summary>
inline bool operator!=( const Vector2& a, const Vector2& b )
{
    return a.x != b.x || a.y != b.y;
}

/// <summary>
/// 反転
/// </summary>
inline Vector2 operator-( const Vector2& a )
{
    return Vector2( -a.x, -a.y );
}

/// <summary>
/// 加算
/// </summary>
inline Vector2 operator+( const Vector2& a, const Vector2& b )
{
    return Vector2( a.x + b.x, a.y + b.y );
}

/// <summary>
/// 減算
/// </summary>
inline Vector2 operator-( const Vector2& a, const Vector2& b )
{
    return Vector2( a.x - b.x, a.y - b.y );
}

/// <summary>
/// 乗算
/// </summary>
inline Vector2 operator*( const Vector2& a, float b )
{
    return Vector2( a.x * b, a.y * b );
}

/// <summary>
/// 乗算
/// </summary>
inline Vector2 operator*( float a, const Vector2& b )
{
    return Vector2( a * b.x, a * b.y );
}

/// <summary>
/// 除算
/// </summary>
inline Vector2 operator/( const Vector2& a, float b )
{
    float denom = 1.0f / b;
    return Vector2( a.x * denom, a.y * denom );
}

/// <summary>
/// 加算
/// </summary>
inline Vector2& operator+=( Vector2& a, const Vector2& b )
{
    a = a + b;
    return a;
}

/// <summary>
/// 減算
/// </summary>
inline Vector2& operator-=( Vector2& a, const Vector2& b )
{
    a = a - b;
    return a;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector2& operator*=( Vector2& a, float b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 除算
/// </summary>
inline Vector2& operator/=( Vector2& a, float b )
{
    float denom = 1.0f / b;
    a *= denom;
    return a;
}

/// <summary>
/// 外積
/// </summary>
inline float Cross( const Vector2& a, const Vector2& b )
{
    return a.x * b.y - a.y * b.x;
}

/// <summary>
/// 内積
/// </summary>
inline float Dot( const Vector2& a, const Vector2& b )
{
    return a.x * b.x + a.y * b.y;
}

/// <summary>
/// 大きさ
/// </summary>
inline float Length( const Vector2& a )
{
    return std::sqrt( a.x * a.x + a.y * a.y );
}

/// <summary>
/// 大きさの2乗
/// </summary>
inline float LengthSq( const Vector2& a )
{
    return a.x * a.x + a.y * a.y;
}

/// <summary>
/// 正規化
/// </summary>
inline Vector2 Normalize( const Vector2& a )
{
    Vector2 vec = a;
    vec.Normalize();
    return vec;
}

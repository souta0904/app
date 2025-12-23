#pragma once
#include <cassert>
#include <cstdint>

#include "Quaternion.h"
#include "Vector3.h"

/// <summary>
/// 4x4行列
/// </summary>
class Matrix4
{
   public:
    float m[4][4];

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Matrix4()
    {
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[0][3] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[1][3] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = 1.0f;
        m[2][3] = 0.0f;
        m[3][0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 0.0f;
        m[3][3] = 1.0f;
    }
};

/// <summary>
/// 乗算
/// </summary>
inline Matrix4 operator*( const Matrix4& a, const Matrix4& b )
{
    Matrix4 mat;
    mat.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0];
    mat.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1];
    mat.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2];
    mat.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3];
    mat.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0];
    mat.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1];
    mat.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2];
    mat.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3];
    mat.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0];
    mat.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1];
    mat.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2];
    mat.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3];
    mat.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0];
    mat.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1];
    mat.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2];
    mat.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3];
    return mat;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3 operator*( const Vector3& a, const Matrix4& b )
{
    Vector3 vec;
    vec.x = a.x * b.m[0][0] + a.y * b.m[1][0] + a.z * b.m[2][0] + b.m[3][0];
    vec.y = a.x * b.m[0][1] + a.y * b.m[1][1] + a.z * b.m[2][1] + b.m[3][1];
    vec.z = a.x * b.m[0][2] + a.y * b.m[1][2] + a.z * b.m[2][2] + b.m[3][2];
    return vec;
}

/// <summary>
/// 乗算
/// </summary>
inline Matrix4 operator*=( Matrix4& a, const Matrix4& b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 乗算
/// </summary>
inline Vector3& operator*=( Vector3& a, const Matrix4& b )
{
    a = a * b;
    return a;
}

/// <summary>
/// 行列式
/// </summary>
inline float Determinant( const Matrix4& a )
{
    return a.m[0][0] * ( ( a.m[1][1] * ( a.m[2][2] * a.m[3][3] - a.m[2][3] * a.m[3][2] ) ) + ( a.m[1][2] * ( a.m[2][3] * a.m[3][1] - a.m[2][1] * a.m[3][3] ) ) + ( a.m[1][3] * ( a.m[2][1] * a.m[3][2] - a.m[2][2] * a.m[3][1] ) ) ) -
           a.m[0][1] * ( ( a.m[1][0] * ( a.m[2][2] * a.m[3][3] - a.m[2][3] * a.m[3][2] ) ) + ( a.m[1][2] * ( a.m[2][3] * a.m[3][0] - a.m[2][0] * a.m[3][3] ) ) + ( a.m[1][3] * ( a.m[2][0] * a.m[3][2] - a.m[2][2] * a.m[3][0] ) ) ) +
           a.m[0][2] * ( ( a.m[1][0] * ( a.m[2][1] * a.m[3][3] - a.m[2][3] * a.m[3][1] ) ) + ( a.m[1][1] * ( a.m[2][3] * a.m[3][0] - a.m[2][0] * a.m[3][3] ) ) + ( a.m[1][3] * ( a.m[2][0] * a.m[3][1] - a.m[2][1] * a.m[3][0] ) ) ) -
           a.m[0][3] * ( ( a.m[1][0] * ( a.m[2][1] * a.m[3][2] - a.m[2][2] * a.m[3][1] ) ) + ( a.m[1][1] * ( a.m[2][2] * a.m[3][0] - a.m[2][0] * a.m[3][2] ) ) + ( a.m[1][2] * ( a.m[2][0] * a.m[3][1] - a.m[2][1] * a.m[3][0] ) ) );
}

/// <summary>
/// 逆行列
/// </summary>
inline Matrix4 Inverse( const Matrix4& a )
{
    float det = Determinant( a );
    //assert( std::fabs( det ) > MathUtil::kEpsilon );
    assert( det != 0.0f );
    float invDet = 1.0f / det;

    Matrix4 mat;
    mat.m[0][0] = ( a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1] ) * invDet;
    mat.m[0][1] = ( a.m[0][2] * a.m[2][1] - a.m[0][1] * a.m[2][2] ) * invDet;
    mat.m[0][2] = ( a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1] ) * invDet;
    mat.m[0][3] = 0.0f;
    mat.m[1][0] = ( a.m[1][2] * a.m[2][0] - a.m[1][0] * a.m[2][2] ) * invDet;
    mat.m[1][1] = ( a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0] ) * invDet;
    mat.m[1][2] = ( a.m[0][2] * a.m[1][0] - a.m[0][0] * a.m[1][2] ) * invDet;
    mat.m[1][3] = 0.0f;
    mat.m[2][0] = ( a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0] ) * invDet;
    mat.m[2][1] = ( a.m[0][1] * a.m[2][0] - a.m[0][0] * a.m[2][1] ) * invDet;
    mat.m[2][2] = ( a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0] ) * invDet;
    mat.m[2][3] = 0.0f;
    mat.m[3][0] = -( a.m[3][0] * mat.m[0][0] + a.m[3][1] * mat.m[1][0] + a.m[3][2] * mat.m[2][0] );
    mat.m[3][1] = -( a.m[3][0] * mat.m[0][1] + a.m[3][1] * mat.m[1][1] + a.m[3][2] * mat.m[2][1] );
    mat.m[3][2] = -( a.m[3][0] * mat.m[0][2] + a.m[3][1] * mat.m[1][2] + a.m[3][2] * mat.m[2][2] );
    mat.m[3][3] = 1.0f;
    return mat;
}

/// <summary>
/// 転置行列
/// </summary>
inline Matrix4 Transpose( const Matrix4& a )
{
    Matrix4 mat;
    mat.m[0][0] = a.m[0][0];
    mat.m[0][1] = a.m[1][0];
    mat.m[0][2] = a.m[2][0];
    mat.m[0][3] = a.m[3][0];
    mat.m[1][0] = a.m[0][1];
    mat.m[1][1] = a.m[1][1];
    mat.m[1][2] = a.m[2][1];
    mat.m[1][3] = a.m[3][1];
    mat.m[2][0] = a.m[0][2];
    mat.m[2][1] = a.m[1][2];
    mat.m[2][2] = a.m[2][2];
    mat.m[2][3] = a.m[3][2];
    mat.m[3][0] = a.m[0][3];
    mat.m[3][1] = a.m[1][3];
    mat.m[3][2] = a.m[2][3];
    mat.m[3][3] = a.m[3][3];
    return mat;
}

/// <summary>
/// スケール行列を作成
/// </summary>
inline Matrix4 CreateScale( const Vector3& scale )
{
    Matrix4 mat;
    mat.m[0][0] = scale.x;
    mat.m[1][1] = scale.y;
    mat.m[2][2] = scale.z;
    return mat;
}

/// <summary>
/// X軸回転行列を作成
/// </summary>
inline Matrix4 CreateRotateX( float rotate )
{
    float cos = std::cos( rotate );
    float sin = std::sin( rotate );
    Matrix4 mat;
    mat.m[1][1] = cos;
    mat.m[1][2] = sin;
    mat.m[2][1] = -sin;
    mat.m[2][2] = cos;
    return mat;
}

/// <summary>
/// Y軸回転行列を作成
/// </summary>
inline Matrix4 CreateRotateY( float rotate )
{
    float cos = std::cos( rotate );
    float sin = std::sin( rotate );
    Matrix4 mat;
    mat.m[0][0] = cos;
    mat.m[0][2] = -sin;
    mat.m[2][0] = sin;
    mat.m[2][2] = cos;
    return mat;
}

/// <summary>
/// Z軸回転行列を作成
/// </summary>
inline Matrix4 CreateRotateZ( float rotate )
{
    float cos = std::cos( rotate );
    float sin = std::sin( rotate );
    Matrix4 mat;
    mat.m[0][0] = cos;
    mat.m[0][1] = sin;
    mat.m[1][0] = -sin;
    mat.m[1][1] = cos;
    return mat;
}

/// <summary>
/// 回転行列を作成
/// </summary>
inline Matrix4 CreateRotate( const Quaternion& q )
{
    float ww = 2.0f * q.w;
    float xx = 2.0f * q.x;
    float yy = 2.0f * q.y;
    float zz = 2.0f * q.z;
    Matrix4 quat;
    quat.m[0][0] = 1.0f - yy * q.y - zz * q.z;
    quat.m[0][1] = xx * q.y + ww * q.z;
    quat.m[0][2] = xx * q.z - ww * q.y;
    quat.m[1][0] = xx * q.y - ww * q.z;
    quat.m[1][1] = 1.0f - xx * q.x - zz * q.z;
    quat.m[1][2] = yy * q.z + ww * q.x;
    quat.m[2][0] = xx * q.z + ww * q.y;
    quat.m[2][1] = yy * q.z - ww * q.x;
    quat.m[2][2] = 1.0f - xx * q.x - yy * q.y;
    return quat;
}

/// <summary>
/// 平行移動行列を作成
/// </summary>
inline Matrix4 CreateTranslate( const Vector3& translate )
{
    Matrix4 mat;
    mat.m[3][0] = translate.x;
    mat.m[3][1] = translate.y;
    mat.m[3][2] = translate.z;
    return mat;
}

/// <summary>
/// アフィン変換行列を作成
/// </summary>
inline Matrix4 CreateAffine( const Vector3& scale, const Quaternion& rotate, const Vector3& translate )
{
    return CreateScale( scale ) * CreateRotate( rotate ) * CreateTranslate( translate );
}

/// <summary>
/// ビュー行列を作成
/// </summary>
inline Matrix4 CreateLookAt( const Vector3& eye, const Vector3& target, const Vector3& up )
{
    Vector3 z = Normalize( target - eye );
    Vector3 x = Normalize( Cross( up, z ) );
    Vector3 y = Normalize( Cross( z, x ) );
    Matrix4 mat;
    mat.m[0][0] = x.x;
    mat.m[0][1] = y.x;
    mat.m[0][2] = z.x;
    mat.m[1][0] = x.y;
    mat.m[1][1] = y.y;
    mat.m[1][2] = z.y;
    mat.m[2][0] = x.z;
    mat.m[2][1] = y.z;
    mat.m[2][2] = z.z;
    mat.m[3][0] = -Dot( x, eye );
    mat.m[3][1] = -Dot( y, eye );
    mat.m[3][2] = -Dot( z, eye );
    return mat;
}

/// <summary>
/// 正射影行列を作成
/// </summary>
inline Matrix4 CreateOrthographic( float left, float top, float right, float bottom, float nearZ, float farZ )
{
    Matrix4 mat;
    mat.m[0][0] = 2.0f / ( right - left );
    mat.m[1][1] = 2.0f / ( top - bottom );
    mat.m[2][2] = 1.0f / ( farZ - nearZ );
    mat.m[3][0] = ( left + right ) / ( left - right );
    mat.m[3][1] = ( top + bottom ) / ( bottom - top );
    mat.m[3][2] = nearZ / ( nearZ - farZ );
    return mat;
}

/// <summary>
/// 透視投影行列を作成
/// </summary>
inline Matrix4 CreatePerspectiveFov( float fovY, float aspectRatio, float nearZ, float farZ )
{
    float yScale = 1.0f / std::tan( fovY * 0.5f );
    Matrix4 mat;
    mat.m[0][0] = yScale / aspectRatio;
    mat.m[1][1] = yScale;
    mat.m[2][2] = farZ / ( farZ - nearZ );
    mat.m[2][3] = 1.0f;
    mat.m[3][2] = ( -nearZ * farZ ) / ( farZ - nearZ );
    mat.m[3][3] = 0.0f;
    return mat;
}

/// <summary>
/// スケールを取得
/// </summary>
inline Vector3 GetScale( const Matrix4& a )
{
    Vector3 vec;
    vec.x = std::sqrt( a.m[0][0] * a.m[0][0] + a.m[0][1] * a.m[0][1] + a.m[0][2] * a.m[0][2] );
    vec.y = std::sqrt( a.m[1][0] * a.m[1][0] + a.m[1][1] * a.m[1][1] + a.m[1][2] * a.m[1][2] );
    vec.z = std::sqrt( a.m[2][0] * a.m[2][0] + a.m[2][1] * a.m[2][1] + a.m[2][2] * a.m[2][2] );
    return vec;
}

/// <summary>
/// 平行移動を取得
/// </summary>
inline Vector3 GetTranslate( const Matrix4& a )
{
    return Vector3( a.m[3][0], a.m[3][1], a.m[3][2] );
}

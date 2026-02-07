#pragma once
#include "math/Matrix4.h"
#include "math/Primitive.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"

/// <summary>
/// カメラ
/// </summary>
class Camera
{
   public:
    /// <summary>
    /// プロジェクション行列種別
    /// </summary>
    enum class ProjectionMode
    {
        Orthographic,
        Perspective,
    };

    // プロジェクション行列種別
    ProjectionMode mProjectionMode;
    // 回転
    Quaternion mRotate;
    // 座標
    Vector3 mPosition;
    // 視野角
    float mFov;
    // 近平面
    float mNearZ;
    // 遠平面
    float mFarZ;

   private:
    // ビュー行列
    Matrix4 mView;
    // プロジェクション行列
    Matrix4 mProjection;

    Frustum mFrustum;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Camera();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Camera() = default;

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>ビュー行列を取得</summary>
    const Matrix4& GetView() const { return mView; }

    /// <summary>プロジェクション行列を取得</summary>
    const Matrix4& GetProjection() const { return mProjection; }

    /// <summary>視錐台を取得</summary>
    const Frustum& GetFrustum() const { return mFrustum; }
};

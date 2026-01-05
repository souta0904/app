#pragma once
#include <memory>

#include "Camera.h"
#include "input/InputState.h"

/// <summary>
/// デバッグ用カメラ
/// </summary>
class DebugCamera
{
   private:
    // カメラ
    std::unique_ptr<Camera> mCamera;
    // ヨー
    float mYaw;
    // ピッチ
    float mPitch;
    // 平行移動速度
    Vector2 mVelocity;
    // 距離
    float mDistance;
    // 注視点
    Vector3 mTarget;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    DebugCamera();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~DebugCamera() = default;

    /// <summary>
    /// 入力処理
    /// </summary>
    /// <param name="state">入力状態</param>
    void Input( const InputState& state );

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>カメラの取得</summary>
    Camera* GetCamera() const { return mCamera.get(); }
};

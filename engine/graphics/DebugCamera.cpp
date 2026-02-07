#include "DebugCamera.h"

#include <algorithm>

#include "core/Window.h"

// コンストラクタ
DebugCamera::DebugCamera()
    : mCamera( nullptr )
    , mYaw( 0.0f )
    , mPitch( 10.0f * MathUtil::kDegToRad )
    , mVelocity( Vector2::kZero )
    , mDistance( 100.0f )
    , mTarget( Vector3( 0.0f, 5.0f, 0.0f ) )
{
    mCamera = std::make_unique<Camera>();
}

// 入力処理
void DebugCamera::Input( const InputState& state )
{
    auto mouse = state.mMouse;
    auto moveDelta = mouse.GetMoveDelta();

    // 回転
    if( mouse.GetButton( 1 ) )
    {
        const auto kRotateSpeed = 0.002f;
        mYaw += moveDelta.x * kRotateSpeed;
        mPitch += moveDelta.y * kRotateSpeed;  // moveDelta.yは上がマイナス
    }

    // 平行移動
    mVelocity = Vector2::kZero;
    if( mouse.GetButton( 2 ) )
    {
        const auto kPanSpeed = 0.1f;
        mVelocity = Vector2( -moveDelta.x * kPanSpeed, moveDelta.y * kPanSpeed );
    }

    // ドリーイン/ドリーアウト
    const auto kDollySpeed = 0.005f;
    mDistance -= mouse.GetWheelDelta() * kDollySpeed;
    const auto kMinDistance = 0.0f;
    const auto kMaxDistance = 1000.0f;
    mDistance = std::clamp( mDistance, kMinDistance, kMaxDistance );
}

// 更新
void DebugCamera::Update()
{
    const auto kPitchLimit = 85.0f * MathUtil::kDegToRad;
    mPitch = std::clamp( mPitch, -kPitchLimit, kPitchLimit );

    Quaternion yawRot( Vector3( 0.0f, 1.0f, 0.0f ), mYaw );
    Quaternion pitchRot( Vector3( 1.0f, 0.0f, 0.0f ), mPitch );
    auto cameraRot = pitchRot * yawRot;
    cameraRot.Normalize();
    auto invRot = Conjugate( cameraRot );

    mTarget += Vector3( mVelocity.x, mVelocity.y, 0.0f ) * invRot;

    mCamera->mPosition = mTarget + Vector3( 0.0f, 0.0f, -mDistance ) * invRot;
    mCamera->mRotate = cameraRot;
    mCamera->Update();
}

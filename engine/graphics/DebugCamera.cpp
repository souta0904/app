#include "DebugCamera.h"

#include <algorithm>

#include "core/Window.h"

// コンストラクタ
DebugCamera::DebugCamera()
    : mCamera( nullptr )
    , mYaw( 0.0f )
    , mPitch( 10.0f * MathUtil::kDegToRad )
    , mPanVel( Vector2::kZero )
    , mDistance( 20.0f )
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
        mPitch += moveDelta.y * kRotateSpeed;
    }

    // パン
    mPanVel = Vector2::kZero;
    if( mouse.GetButton( 2 ) )
    {
        const auto kPanSpeed = 0.05f;
        mPanVel = Vector2( -moveDelta.x * kPanSpeed, moveDelta.y * kPanSpeed );
    }

    // ドリー
    const auto kDollySpeed = 0.002f;
    mDistance -= mouse.GetWheelDelta() * kDollySpeed;
    const auto kMinDistance = 0.0f;
    const auto kMaxDistance = 500.0f;
    mDistance = std::clamp( mDistance, kMinDistance, kMaxDistance );
}

// 更新
void DebugCamera::Update()
{
    const auto kPitchLimit = 89.0f * MathUtil::kDegToRad;
    mPitch = std::clamp( mPitch, -kPitchLimit, kPitchLimit );

    // 回転
    Quaternion yawRot( Vector3( 0.0f, 1.0f, 0.0f ), mYaw );
    Quaternion pitchRot( Vector3( 1.0f, 0.0f, 0.0f ), mPitch );
    auto cameraRot = pitchRot * yawRot;
    cameraRot.Normalize();
    auto invRot = Conjugate( cameraRot );

    // パン
    mTarget += Vector3( mPanVel.x, mPanVel.y, 0.0f ) * invRot;

    mCamera->mPosition = mTarget + Vector3( 0.0f, 0.0f, -mDistance ) * invRot;
    mCamera->mRotate = cameraRot;
    mCamera->Update();
}

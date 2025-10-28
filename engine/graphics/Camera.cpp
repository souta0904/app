#include "Camera.h"

#include "core/Window.h"

// コンストラクタ
Camera::Camera()
    : mProjectionMode( ProjectionMode::Perspective )
    , mRotate()
    , mPosition( Vector3::kZero )
    , mFov( MathUtil::kPi * 0.5f )
    , mNearZ( 0.1f )
    , mFarZ( 1000.0f )
    , mView()
    , mProjection()
{
}

// 更新
void Camera::Update()
{
    // ビュー行列を更新
    auto worldMat = CreateRotate( mRotate ) * CreateTranslate( mPosition );
    mView = Inverse( worldMat );

    // プロジェクション行列を更新
    auto& window = Window::GetInstance();
    auto windowWidth = static_cast<float>( window.GetWidth() );
    auto windowHeight = static_cast<float>( window.GetHeight() );
    switch( mProjectionMode )
    {
        case ProjectionMode::Orthographic:
            mProjection = CreateOrthographic(
                0.0f,
                0.0f,
                windowWidth,
                windowHeight,
                mNearZ,
                mFarZ );
            break;

        case ProjectionMode::Perspective:
            mProjection = CreatePerspectiveFov(
                mFov,
                windowWidth / windowHeight,
                mNearZ,
                mFarZ );
            break;
    }
}

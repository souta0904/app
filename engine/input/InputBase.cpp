#include "InputBase.h"

#include "core/Window.h"

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "xinput.lib" )

// コンストラクタ
InputBase::InputBase()
    : mHWnd( nullptr )
    , mDirectInput( nullptr )
    , mState()
{
}

// 初期化
bool InputBase::Init()
{
    auto& window = Window::GetInstance();
    mHWnd = window.GetHWnd();

    // DirectInputの作成
    auto hr = DirectInput8Create(
        window.GetHInstance(),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        reinterpret_cast<void**>( mDirectInput.GetAddressOf() ),
        nullptr );
    if( FAILED( hr ) )
    {
        return false;
    }

    /* ---------- キーボード ---------- */

    hr = mDirectInput->CreateDevice(
        GUID_SysKeyboard,
        mState.mKeyboard.mDevice.GetAddressOf(),
        nullptr );
    if( FAILED( hr ) )
    {
        return false;
    }

    hr = mState.mKeyboard.mDevice->SetDataFormat( &c_dfDIKeyboard );
    if( FAILED( hr ) )
    {
        return false;
    }

    hr = mState.mKeyboard.mDevice->SetCooperativeLevel(
        mHWnd,
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY );
    if( FAILED( hr ) )
    {
        return false;
    }

    mState.mKeyboard.mDevice->Acquire();

    /* ---------- マウス ---------- */

    hr = mDirectInput->CreateDevice(
        GUID_SysMouse,
        mState.mMouse.mDevice.GetAddressOf(),
        nullptr );
    if( FAILED( hr ) )
    {
        return false;
    }

    hr = mState.mMouse.mDevice->SetDataFormat( &c_dfDIMouse2 );
    if( FAILED( hr ) )
    {
        return false;
    }

    hr = mState.mMouse.mDevice->SetCooperativeLevel(
        mHWnd,
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
    if( FAILED( hr ) )
    {
        return false;
    }

    mState.mMouse.mDevice->Acquire();

    return true;
}

// 終了処理
void InputBase::Term()
{
    if( mState.mMouse.mDevice )
    {
        mState.mMouse.mDevice->Unacquire();
    }
    if( mState.mKeyboard.mDevice )
    {
        mState.mKeyboard.mDevice->Unacquire();
    }
}

// 更新
void InputBase::Update()
{
    /* ---------- キーボード ---------- */

    auto& keyboard = mState.mKeyboard;

    memcpy( keyboard.mPrevKeys, keyboard.mCurrKeys, sizeof( keyboard.mCurrKeys ) );
    auto hr = keyboard.mDevice->GetDeviceState( sizeof( keyboard.mCurrKeys ), keyboard.mCurrKeys );
    if( FAILED( hr ) )
    {
        // 再取得
        keyboard.mDevice->Acquire();
        keyboard.mDevice->GetDeviceState( sizeof( keyboard.mCurrKeys ), keyboard.mCurrKeys );
    }

    /* ---------- マウス ---------- */

    auto& mouse = mState.mMouse;

    memcpy( mouse.mPrevButtons, mouse.mCurrButtons, sizeof( mouse.mCurrButtons ) );
    DIMOUSESTATE2 mouseState = {};
    hr = mouse.mDevice->GetDeviceState( sizeof( mouseState ), &mouseState );
    if( FAILED( hr ) )
    {
        // 再取得
        mouse.mDevice->Acquire();
        mouse.mDevice->GetDeviceState( sizeof( mouseState ), &mouseState );
    }

    memcpy( mouse.mCurrButtons, mouseState.rgbButtons, sizeof( mouse.mCurrButtons ) );
    mouse.mMoveDelta.x = static_cast<float>( mouseState.lX );
    mouse.mMoveDelta.y = static_cast<float>( mouseState.lY );
    mouse.mWheelDelta = static_cast<float>( mouseState.lZ );

    POINT mousePos = {};
    GetCursorPos( &mousePos );
    ScreenToClient( mHWnd, &mousePos );  // クライアント座標へ
    mouse.mPosition.x = static_cast<float>( mousePos.x );
    mouse.mPosition.y = static_cast<float>( mousePos.y );

    /* ---------- ゲームパッド ---------- */

    auto& pad = mState.mGamepad;

    pad.mPrevButtons = pad.mCurrButtons;

    XINPUT_STATE padState = {};
    auto result = XInputGetState( 0, &padState );
    if( result == ERROR_SUCCESS )
    {
        pad.mIsConnected = true;
        pad.mCurrButtons = padState.Gamepad.wButtons;
        pad.mLStick = pad.NormalizeStick( padState.Gamepad.sThumbLX, padState.Gamepad.sThumbLY, pad.mLStickDeadzone );
        pad.mRStick = pad.NormalizeStick( padState.Gamepad.sThumbRX, padState.Gamepad.sThumbRY, pad.mRStickDeadzone );
        pad.mLTrigger = pad.NormalizeTrigger( padState.Gamepad.bLeftTrigger, pad.mTriggerDeadzone );
        pad.mRTrigger = pad.NormalizeTrigger( padState.Gamepad.bRightTrigger, pad.mTriggerDeadzone );
    }
    else
    {
        pad.mIsConnected = false;
        pad.mCurrButtons = 0;
        pad.mLStick = Vector2::kZero;
        pad.mRStick = Vector2::kZero;
        pad.mLTrigger = 0.0f;
        pad.mRTrigger = 0.0f;
    }
}

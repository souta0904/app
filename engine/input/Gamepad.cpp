#include "Gamepad.h"

#include <algorithm>

namespace
{

constexpr auto kMaxStickVal = 32767.0f;
constexpr auto kMaxTriggerVal = 255.0f;

}  // namespace

// コンストラクタ
Gamepad::Gamepad()
    : mIsConnected( false )
    , mCurrButtons( 0 )
    , mPrevButtons( 0 )
    , mLStick( Vector2::kZero )
    , mRStick( Vector2::kZero )
    , mLTrigger( 0.0f )
    , mRTrigger( 0.0f )
    , mLStickDeadzone( XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
    , mRStickDeadzone( XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
    , mTriggerDeadzone( XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
{
}

// ボタンを押しているか
bool Gamepad::GetButton( WORD button ) const
{
    return ( mCurrButtons & button ) != 0;
}

// ボタンを離した瞬間か
bool Gamepad::GetButtonUp( WORD button ) const
{
    return ( ( mCurrButtons & button ) == 0 ) &&
           ( ( mPrevButtons & button ) != 0 );
}

// ボタンを押した瞬間か
bool Gamepad::GetButtonDown( WORD button ) const
{
    return ( ( mCurrButtons & button ) != 0 ) &&
           ( ( mPrevButtons & button ) == 0 );
}

// スティック入力を正規化(-1.0～1.0)
Vector2 Gamepad::NormalizeStick( SHORT inputX, SHORT inputY, float deadzone ) const
{
    auto x = static_cast<float>( inputX );
    auto y = static_cast<float>( inputY );
    auto len = std::sqrt( x * x + y * y );
    if( len <= deadzone )
    {
        return Vector2::kZero;
    }
    auto norm = ( len - deadzone ) / ( kMaxStickVal - deadzone );
    norm = std::clamp( norm, 0.0f, 1.0f );
    return Vector2( x / len * norm, y / len * norm );
}

// トリガー入力を正規化(0.0～1.0)
float Gamepad::NormalizeTrigger( BYTE input, float deadzone ) const
{
    if( input <= deadzone )
    {
        return 0.0f;
    }
    return static_cast<float>( input - deadzone ) / ( kMaxTriggerVal - deadzone );
}

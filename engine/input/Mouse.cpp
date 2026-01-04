#include "Mouse.h"

#include <cassert>

// コンストラクタ
Mouse::Mouse()
    : mDevice( nullptr )
    , mCurrButtons()
    , mPrevButtons()
    , mMoveDelta( Vector2::kZero )
    , mWheelDelta( 0.0f )
    , mPosition( Vector2::kZero )
{
}

// ボタンを押しているか
bool Mouse::GetButton( uint8_t button ) const
{
    assert( button < _countof( mCurrButtons ) );
    return ( mCurrButtons[button] & 0x80 ) != 0;
}

// ボタンを離した瞬間か
bool Mouse::GetButtonUp( uint8_t button ) const
{
    assert( button < _countof( mCurrButtons ) );
    return ( ( mCurrButtons[button] & 0x80 ) == 0 ) &&
           ( ( mPrevButtons[button] & 0x80 ) != 0 );
}

// ボタンを押した瞬間か
bool Mouse::GetButtonDown( uint8_t button ) const
{
    assert( button < _countof( mCurrButtons ) );
    return ( ( mCurrButtons[button] & 0x80 ) != 0 ) &&
           ( ( mPrevButtons[button] & 0x80 ) == 0 );
}

#include "Keyboard.h"

// コンストラクタ
Keyboard::Keyboard()
    : mDevice( nullptr )
    , mCurrKeys()
    , mPrevKeys()
{
}

// キーを押しているか
bool Keyboard::GetKey( uint8_t keyCode ) const
{
    return ( mCurrKeys[keyCode] & 0x80 ) != 0;
}

// キーを離した瞬間か
bool Keyboard::GetKeyUp( uint8_t keyCode ) const
{
    return ( ( mCurrKeys[keyCode] & 0x80 ) == 0 ) &&
           ( ( mPrevKeys[keyCode] & 0x80 ) != 0 );
}

// キーを押した瞬間か
bool Keyboard::GetKeyDown( uint8_t keyCode ) const
{
    return ( ( mCurrKeys[keyCode] & 0x80 ) != 0 ) &&
           ( ( mPrevKeys[keyCode] & 0x80 ) == 0 );
}

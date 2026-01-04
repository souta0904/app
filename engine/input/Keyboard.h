#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#include "math/Vector2.h"

/// <summary>
/// キーボード
/// </summary>
class Keyboard
{
    friend class InputBase;

   private:
    // キーボードデバイス
    Microsoft::WRL::ComPtr<IDirectInputDevice8> mDevice;
    // 今のキー状態
    uint8_t mCurrKeys[256];
    // 前のキー状態
    uint8_t mPrevKeys[256];

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Keyboard();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Keyboard() = default;

    /// <summary>
    /// キーを押しているか
    /// </summary>
    /// <param name="keyCode">キー</param>
    /// <returns>押しているか</returns>
    bool GetKey( uint8_t keyCode ) const;

    /// <summary>
    /// キーを離した瞬間か
    /// </summary>
    /// <param name="keyCode">キー</param>
    /// <returns>離した瞬間か</returns>
    bool GetKeyUp( uint8_t keyCode ) const;

    /// <summary>
    /// キーを押した瞬間か
    /// </summary>
    /// <param name="keyCode">キー</param>
    /// <returns>押した瞬間か</returns>
    bool GetKeyDown( uint8_t keyCode ) const;
};

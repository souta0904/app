#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#include "math/Vector2.h"

/// <summary>
/// マウス
/// </summary>
class Mouse
{
    friend class InputBase;

   private:
    // マウスデバイス
    Microsoft::WRL::ComPtr<IDirectInputDevice8> mDevice;
    // 今のボタン状態
    BYTE mCurrButtons[8];
    // 前のボタン状態
    BYTE mPrevButtons[8];
    // 移動量
    Vector2 mMoveDelta;
    // ホイール移動量
    float mWheelDelta;
    // 座標
    Vector2 mPosition;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Mouse();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Mouse() = default;

    /// <summary>
    /// ボタンを押しているか
    /// </summary>
    /// <param name="button">
    /// 0: 左ボタン
    /// 1: 右ボタン
    /// 2: 中ボタン
    /// </param>
    /// <returns>押しているか</returns>
    bool GetButton( uint8_t button ) const;

    /// <summary>
    /// ボタンを離した瞬間か
    /// </summary>
    /// <param name="button">
    /// 0: 左ボタン
    /// 1: 右ボタン
    /// 2: 中ボタン
    /// </param>
    /// <returns>離した瞬間か</returns>
    bool GetButtonUp( uint8_t button ) const;

    /// <summary>
    /// ボタンを押した瞬間か
    /// </summary>
    /// <param name="button">
    /// 0: 左ボタン
    /// 1: 右ボタン
    /// 2: 中ボタン
    /// </param>
    /// <returns>押した瞬間か</returns>
    bool GetButtonDown( uint8_t button ) const;

    /// <summary>移動量を取得</summary>
    const Vector2& GetMoveDelta() const { return mMoveDelta; }

    /// <summary>ホイール移動量を取得</summary>
    float GetWheelDelta() const { return mWheelDelta; }

    /// <summary>座標を取得</summary>
    const Vector2& GetPosition() const { return mPosition; }
};

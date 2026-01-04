#pragma once
#include <Windows.h>
#include <Xinput.h>

#include "math/Vector2.h"

/// <summary>
/// パッド
/// </summary>
class Gamepad
{
    friend class InputBase;

   private:
    // 接続しているか
    bool mIsConnected;

    // 今のボタン状態
    WORD mCurrButtons;
    // 前のボタン状態
    WORD mPrevButtons;
    // 左スティック
    Vector2 mLStick;
    // 右スティック
    Vector2 mRStick;
    // 左トリガー
    float mLTrigger;
    // 右トリガー
    float mRTrigger;

    // 左スティックのデッドゾーン
    float mLStickDeadzone;
    // 右スティックのデッドゾーン
    float mRStickDeadzone;
    // トリガーのデッドゾーン
    float mTriggerDeadzone;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Gamepad();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Gamepad() = default;

    /// <summary>
    /// ボタンを押しているか
    /// </summary>
    /// <param name="button">ボタン種別</param>
    /// <returns>押しているか</returns>
    bool GetButton( WORD button ) const;

    /// <summary>
    /// ボタンを離した瞬間か
    /// </summary>
    /// <param name="button">ボタン種別</param>
    /// <returns>離した瞬間か</returns>
    bool GetButtonUp( WORD button ) const;

    /// <summary>
    /// ボタンを押した瞬間か
    /// </summary>
    /// <param name="button">ボタン種別</param>
    /// <returns>押した瞬間か</returns>
    bool GetButtonDown( WORD button ) const;

    /// <summary>接続しているか</summary>
    bool IsConnected() const { return mIsConnected; }

    /// <summary>左スティックの取得</summary>
    const Vector2& GetLStick() const { return mLStick; }

    /// <summary>右スティックの取得</summary>
    const Vector2& GetRStick() const { return mRStick; }

    /// <summary>左トリガーの取得</summary>
    float GetLTrigger() const { return mLTrigger; }

    /// <summary>右トリガーの取得</summary>
    float GetRTrigger() const { return mRTrigger; }

    /// <summary>左スティックのデッドゾーンを設定</summary>
    void SetLStickDeadzone( float deadzone ) { mLStickDeadzone = deadzone; }

    /// <summary>右スティックのデッドゾーンを設定</summary>
    void SetRStickDeadzone( float deadzone ) { mRStickDeadzone = deadzone; }

    /// <summary>トリガーのデッドゾーンを設定</summary>
    void SetTriggerDeadzone( float deadzone ) { mTriggerDeadzone = deadzone; }

   private:
    /// <summary>
    /// スティック入力を正規化(-1.0～1.0)
    /// </summary>
    /// <param name="inputX">入力X</param>
    /// <param name="inputY">入力Y</param>
    /// <param name="deadzone">デッドゾーン</param>
    /// <returns>正規化されたスティック入力</returns>
    Vector2 NormalizeStick( SHORT inputX, SHORT inputY, float deadzone ) const;

    /// <summary>
    /// トリガー入力を正規化(0.0～1.0)
    /// </summary>
    /// <param name="input">入力</param>
    /// <param name="deadzone">デッドゾーン</param>
    /// <returns>正規化されたトリガー入力</returns>
    float NormalizeTrigger( BYTE input, float deadzone ) const;
};

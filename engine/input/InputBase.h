#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#include <cstdint>

#include "InputState.h"

/// <summary>
/// 入力基盤
/// </summary>
class InputBase
{
   private:
    // ウィンドウハンドル
    HWND mHWnd;
    // DirectInputインターフェース
    Microsoft::WRL::ComPtr<IDirectInput8> mDirectInput;

    // 入力状態
    InputState mState;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static InputBase& GetInstance()
    {
        static InputBase instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    InputBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~InputBase() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    InputBase( const InputBase& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    InputBase& operator=( const InputBase& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    InputBase( InputBase&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    InputBase& operator=( InputBase&& ) = delete;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>入力状態の取得</summary>
    const InputState& GetState() const { return mState; }
};

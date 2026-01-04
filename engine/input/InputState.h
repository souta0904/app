#pragma once
#include "Gamepad.h"
#include "Keyboard.h"
#include "Mouse.h"

/// <summary>
/// 入力状態
/// </summary>
struct InputState
{
    // キーボード
    Keyboard mKeyboard;
    // マウス
    Mouse mMouse;
    // ゲームパッド
    Gamepad mGamepad;
};

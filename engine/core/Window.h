#pragma once
#include <Windows.h>

#include <cstdint>
#include <string>

#include "utils/Singleton.h"

namespace Nebula
{

/// <summary>
/// ウィンドウ
/// </summary>
class Window final : public Singleton<Window>
{
    friend class Singleton<Window>;

   private:
    HINSTANCE mHInstance;
    HWND mHWnd;

    // ウィンドウ幅
    uint32_t mWidth;
    // ウィンドウ高
    uint32_t mHeight;

    // タイトル
    std::wstring mTitle;

   public:
    /// <summary>
    /// ウィンドウプロシージャ
    /// </summary>
    static LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

   private:
    Window();
    ~Window() = default;

   public:
    bool Create( uint32_t width, uint32_t height, const std::wstring& title );
    void Destroy();

    /// <summary>
    /// メッセージの処理
    /// </summary>
    /// <returns>ループ終了でtrue、継続でfalse</returns>
    bool ProcessMessage();

    HINSTANCE GetHInstance() const { return mHInstance; }
    HWND GetHWnd() const { return mHWnd; }
    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
};

}  // namespace Nebula

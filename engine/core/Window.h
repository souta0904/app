#pragma once
#include <Windows.h>

#include <cstdint>
#include <string>

/// <summary>
/// ウィンドウ
/// </summary>
class Window
{
   private:
    // インスタンスハンドル
    HINSTANCE mHInstance;
    // ウィンドウハンドル
    HWND mHWnd;

    // ウィンドウの幅
    uint32_t mWidth;
    // ウィンドウの高さ
    uint32_t mHeight;
    // タイトルバーの文字列
    std::wstring mTitle;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static Window& GetInstance()
    {
        static Window instance;
        return instance;
    }

    /// <summary>
    /// ウィンドウプロシージャ
    /// </summary>
    /// <param name="hwnd">ウィンドウハンドル</param>
    /// <param name="msg">メッセージ</param>
    /// <param name="wp">パラメータ1</param>
    /// <param name="lp">パラメータ2</param>
    /// <returns>処理結果</returns>
    static LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Window();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Window() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    Window( const Window& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    Window& operator=( const Window& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    Window( Window&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    Window& operator=( Window&& ) = delete;

    /// <summary>
    /// ウィンドウを作成
    /// </summary>
    /// <param name="width">ウィンドウの幅</param>
    /// <param name="height">ウィンドウの高さ</param>
    /// <param name="title">タイトルバーの文字列</param>
    /// <returns>成否</returns>
    bool Create( uint32_t width, uint32_t height, const std::wstring& title );

    /// <summary>
    /// ウィンドウを破棄
    /// </summary>
    void Destroy();

    /// <summary>
    /// メッセージを処理
    /// </summary>
    /// <returns>ループ終了か</returns>
    bool ProcessMessage();

    /// <summary>インスタンスハンドルを取得</summary>
    HINSTANCE GetHInstance() const { return mHInstance; }

    /// <summary>ウィンドウハンドルを取得</summary>
    HWND GetHWnd() const { return mHWnd; }

    /// <summary>ウィンドウの幅を取得</summary>
    uint32_t GetWidth() const { return mWidth; }

    /// <summary>ウィンドウの高さを取得</summary>
    uint32_t GetHeight() const { return mHeight; }
};

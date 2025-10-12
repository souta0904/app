#include "Window.h"

#include "imgui/imgui_impl_win32.h"

namespace
{

// ウィンドウクラス名
const auto kClassName = TEXT( "DxWndClass" );

}  // namespace

// imguiのウィンドウプロシージャ
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// ウィンドウプロシージャ
LRESULT CALLBACK Window::WndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
    if( ImGui_ImplWin32_WndProcHandler( hwnd, msg, wp, lp ) )
    {
        return true;
    }

    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }
    return DefWindowProc( hwnd, msg, wp, lp );
}

// コンストラクタ
Window::Window()
    : mHInstance( nullptr )
    , mHWnd( nullptr )
    , mWidth( 0 )
    , mHeight( 0 )
    , mTitle()
{
}

// ウィンドウを作成
bool Window::Create( uint32_t width, uint32_t height, const std::wstring& title )
{
    mWidth = width;
    mHeight = height;
    mTitle = title;

    // インスタンスハンドルを取得
    mHInstance = GetModuleHandle( nullptr );
    if( !mHInstance ) return false;

    // ウィンドウクラスを設定
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof( WNDCLASSEX );
    wc.lpfnWndProc = WndProc;
    wc.hInstance = mHInstance;
    wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wc.lpszClassName = kClassName;
    if( !RegisterClassEx( &wc ) ) return false;

    // ウィンドウスタイルを考慮してウィンドウサイズを計算
    RECT rc = { 0, 0, static_cast<LONG>( mWidth ), static_cast<LONG>( mHeight ) };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, false );

    // ウィンドウを作成
    mHWnd = CreateWindow(
        wc.lpszClassName,
        mTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        mHInstance,
        nullptr );
    if( !mHWnd ) return false;

    // ウィンドウを表示
    ShowWindow( mHWnd, SW_SHOW );

    return true;
}

// ウィンドウを破棄
void Window::Destroy()
{
    DestroyWindow( mHWnd );
    if( mHInstance )
    {
        UnregisterClass( kClassName, mHInstance );
    }
    mHInstance = nullptr;
    mHWnd = nullptr;
}

// メッセージを処理
bool Window::ProcessMessage()
{
    MSG msg = {};
    while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        if( msg.message == WM_QUIT )
        {
            // ループ終了
            return true;
        }
    }
    return false;
}

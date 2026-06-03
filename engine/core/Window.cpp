#include "Window.h"

#include "imgui/imgui_impl_win32.h"

namespace
{

// ウィンドウクラス名
const auto kClassName = TEXT( "DxWndClass" );

}  // namespace

// ImGuiのウィンドウプロシージャの前方宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace Nebula
{

// 独自のウィンドウプロシージャ
LRESULT CALLBACK Window::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) ) return true;

    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

Window::Window()
    : mHInstance( nullptr )
    , mHWnd( nullptr )
    , mWidth( 0 )
    , mHeight( 0 )
    , mTitle()
{
}

bool Window::Create( uint32_t width, uint32_t height, const std::wstring& title )
{
    if( mHWnd ) return false;  // 作成済みチェック

    mWidth = width;
    mHeight = height;
    mTitle = title;

    mHInstance = GetModuleHandle( nullptr );
    if( !mHInstance ) return false;

    WNDCLASSEX wndClass = {};
    wndClass.cbSize = sizeof( WNDCLASSEX );
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = mHInstance;
    wndClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wndClass.lpszClassName = kClassName;
    if( !RegisterClassEx( &wndClass ) ) return false;

    // クライアント領域のサイズからウィンドウのサイズを計算
    RECT wndRc = { 0, 0, static_cast<LONG>( mWidth ), static_cast<LONG>( mHeight ) };
    AdjustWindowRect( &wndRc, WS_OVERLAPPEDWINDOW, false );

    mHWnd = CreateWindow(
        wndClass.lpszClassName,
        mTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wndRc.right - wndRc.left,
        wndRc.bottom - wndRc.top,
        nullptr,
        nullptr,
        mHInstance,
        nullptr );
    if( !mHWnd ) return false;

    ShowWindow( mHWnd, SW_SHOW );

    return true;
}

void Window::Destroy()
{
    // 破棄
    if( mHWnd )
    {
        DestroyWindow( mHWnd );
    }
    if( mHInstance )
    {
        UnregisterClass( kClassName, mHInstance );
    }

    mHInstance = nullptr;
    mHWnd = nullptr;
}

bool Window::ProcessMessage()
{
    MSG msg = {};
    while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        if( msg.message == WM_QUIT ) return true;
    }

    return false;
}

}  // namespace Nebula

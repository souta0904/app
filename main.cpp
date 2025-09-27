#include "Window.h"

// エントリポイント
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    auto& window = Window::GetInstance();

    if( !window.Create( 1920, 1080, L"Game" ) )
    {
        MessageBox( nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }

    // ゲームループ
    while( !window.ProcessMessage() )
    {
        // 毎ループ処理
    }

    window.Destroy();

    return 0;
}

#include "DirectXBase.h"
#include "Window.h"

// エントリポイント
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    auto& window = Window::GetInstance();
    auto& dxBase = DirectXBase::GetInstance();

    if( !window.Create( 1920, 1080, L"Game" ) )
    {
        MessageBox( nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    if( !dxBase.Init() )
    {
        MessageBox( nullptr, L"Failed to initialize DirectX.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }

    // ゲームループ
    while( !window.ProcessMessage() )
    {
        dxBase.BeginDraw();

        dxBase.EndDraw();
    }

    dxBase.Term();
    window.Destroy();

    return 0;
}

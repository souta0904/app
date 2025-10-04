#include <format>

#include "DirectXBase.h"
#include "EditorBase.h"
#include "Logger.h"
#include "StringHelper.h"
#include "Window.h"
#include "imgui/imgui.h"

// エントリポイント
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    auto& window = Window::GetInstance();
    auto& dxBase = DirectXBase::GetInstance();
    auto& editorBase = EditorBase::GetInstance();

    LOG_INFO( "hello, world!" );
    LOG_INFO( "LOG_INFO test." );
    LOG_WARN( "LOG_WARN test." );
    LOG_ERROR( "LOG_ERROR test." );

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
    if( !editorBase.Init() )
    {
        MessageBox( nullptr, L"Failed to initialize editor.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }

    // ゲームループ
    while( !window.ProcessMessage() )
    {
        editorBase.Begin();

        // imguiテストウィンドウ
        ImGui::Begin( "Test" );
        auto text = StringHelper::Convert( std::format( L"Adapter Name: {}", dxBase.GetAdapterName() ) );
        ImGui::Text( text.c_str() );
        auto clearColor = dxBase.GetClearColor();
        ImGui::ColorPicker3( "Clear Color", &clearColor.r );
        dxBase.SetClearColor( clearColor );
        ImGui::End();

        editorBase.End();

        dxBase.BeginDraw();

        editorBase.Draw( dxBase.GetCmdList() );

        dxBase.EndDraw();
    }

    editorBase.Term();
    dxBase.Term();
    window.Destroy();

    return 0;
}

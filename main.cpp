#include <chrono>
#include <deque>
#include <format>

#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "core/GraphicsPSO.h"
#include "core/ResourceManager.h"
#include "core/RootSignature.h"
#include "core/VertexBuffer.h"
#include "core/Window.h"
#include "editor/EditorBase.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"
#include "imgui/imgui.h"
#include "input/InputBase.h"
#include "math/Vector4.h"
#include "utils/Logger.h"
#include "utils/StringHelper.h"

// エントリポイント
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    auto& window = Window::GetInstance();
    auto& dxBase = DirectXBase::GetInstance();
    auto& editorBase = EditorBase::GetInstance();
    auto& resMgr = ResourceManager::GetInstance();
    auto& renderer = Renderer::GetInstance();
    auto& inputBase = InputBase::GetInstance();

    // ウィンドウを作成
    if( !window.Create( 1920, 1080, L"Game" ) )
    {
        LOG_ERROR( "Failed to create window." );
        MessageBox( nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "Window created successfully." );
    }

    // DirectXを初期化
    if( !dxBase.Init() )
    {
        LOG_ERROR( "Failed to initialize DirectX." );
        MessageBox( nullptr, L"Failed to initialize DirectX.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "DirectX initialized successfully." );
    }

    // エディタを初期化
    if( !editorBase.Init() )
    {
        LOG_ERROR( "Failed to initialize editor base." );
        MessageBox( nullptr, L"Failed to initialize editor base.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "Editor base initialized successfully." );
    }

    // リソース管理を初期化
    if( !resMgr.Init() )
    {
        LOG_ERROR( "Failed to initialize resource manager." );
        MessageBox( nullptr, L"Failed to initialize resource manager.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "Resource manager initialized successfully." );
    }

    // レンダラーを初期化
    if( !renderer.Init() )
    {
        LOG_ERROR( "Failed to initialize renderer." );
        MessageBox( nullptr, L"Failed to initialize renderer.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "Renderer initialized successfully." );
    }

    // 入力を初期化
    if( !inputBase.Init() )
    {
        LOG_ERROR( "Failed to initialize input base." );
        MessageBox( nullptr, L"Failed to initialize input base.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "Input base initialized successfully." );
    }

    std::chrono::steady_clock::time_point prevTime = std::chrono::steady_clock::now();
    auto deltaTime = 0.0f;

    std::deque<float> deltaTimes;
    auto totalDeltaTime = 0.0f;
    constexpr auto kFPSInterval = 0.5f;
    auto avgDeltaTime = 0.0f;
    auto minDeltaTime = 0.0f;
    auto maxDeltaTime = 0.0f;

    // ゲームループ
    while( !window.ProcessMessage() )
    {
        // デルタタイムを計測
        auto currTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currTime - prevTime;
        deltaTime = elapsed.count();
        if( deltaTime > 0.1f )
        {
            deltaTime = 0.1f;
        }
        prevTime = currTime;

        // データ計算
        deltaTimes.push_back( deltaTime );
        totalDeltaTime += deltaTime;
        if( totalDeltaTime >= kFPSInterval )
        {
            avgDeltaTime = totalDeltaTime / deltaTimes.size();
            if( !deltaTimes.empty() )
            {
                minDeltaTime = deltaTimes[0];
                maxDeltaTime = deltaTimes[0];
                for( auto dt : deltaTimes )
                {
                    if( minDeltaTime > dt )
                    {
                        minDeltaTime = dt;
                    }
                    if( maxDeltaTime < dt )
                    {
                        maxDeltaTime = dt;
                    }
                }
            }

            deltaTimes.clear();
            totalDeltaTime = 0.0f;
        }

        inputBase.Update();

        renderer.Input( inputBase.GetState() );

        editorBase.Begin();

        // imguiテストウィンドウ
        ImGui::Begin( "Info" );
        auto text = StringHelper::Convert( std::format( L"Adapter Name: {}", dxBase.GetAdapterName() ) );
        ImGui::Text( text.c_str() );
        auto clearColor = dxBase.GetClearColor();
        ImGui::ColorPicker3( "Clear Color", &clearColor.r );
        dxBase.SetClearColor( clearColor );
        ImGui::Text( std::format( "FPS ( Avg ) : {:.1f}", 1.0f / avgDeltaTime ).c_str() );
        ImGui::Text( std::format( "Max FPS : {:.1f}", 1.0f / minDeltaTime ).c_str() );
        ImGui::Text( std::format( "Min FPS : {:.1f}", 1.0f / maxDeltaTime ).c_str() );
        ImGui::Text( std::format( "Delta time ( Avg ) : {:.3f} ms", avgDeltaTime * 1000.0f ).c_str() );
        ImGui::Text( std::format( "Delta time: {:.3f} ms", deltaTime * 1000.0f ).c_str() );
        auto useVSync = dxBase.GetUseVSync();
        ImGui::Checkbox( "Use VSync", &useVSync );
        dxBase.SetUseVSync( useVSync );
        // ImGui::Image( texture->GetSRVHdl()->mGPU.ptr, ImVec2( 256.0f, 256.0f ) );
        ImGui::End();

        renderer.UpdateGUI();

        editorBase.End();

        renderer.Update( deltaTime );

        dxBase.BeginDraw();

        auto cmdList = dxBase.GetCmdList();
        renderer.Draw( cmdList );

        editorBase.Draw( cmdList );

        dxBase.EndDraw();
    }

    inputBase.Term();
    LOG_INFO( "Input base terminated." );

    renderer.Term();
    LOG_INFO( "Renderer terminated." );

    resMgr.Term();
    LOG_INFO( "Resource manager terminated." );

    editorBase.Term();
    LOG_INFO( "Editor base terminated." );

    dxBase.Term();
    LOG_INFO( "DirectX terminated." );

    window.Destroy();
    LOG_INFO( "Window destroyed." );

    return 0;
}

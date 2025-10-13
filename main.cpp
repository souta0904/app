#include <format>

#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "core/GraphicsPSO.h"
#include "core/RootSignature.h"
#include "core/VertexBuffer.h"
#include "core/Window.h"
#include "editor/EditorBase.h"
#include "graphics/ResourceManager.h"
#include "graphics/Texture.h"
#include "imgui/imgui.h"
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
        LOG_ERROR( "Failed to initialize editor." );
        MessageBox( nullptr, L"Failed to initialize editor.", L"Error", MB_OK | MB_ICONERROR );
        return -1;
    }
    else
    {
        LOG_INFO( "Editor initialized successfully." );
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

    // ルートシグネチャを作成
    std::unique_ptr<RootSignature> rs = std::make_unique<RootSignature>();
    rs->Init( 0, 0 );
    if( !rs->Create() )
    {
        return -1;
    }

    // パイプラインステートを作成
    PSOInit init = {};
    init.mRootSignature = rs.get();
    init.mVS = resMgr.GetShader( "assets/shader/SimpleVS.hlsl", "vs_6_0" );
    init.mPS = resMgr.GetShader( "assets/shader/SimplePS.hlsl", "ps_6_0" );
    init.mBlendState = DirectXCommonSettings::gBlendAlpha;
    init.mRasterizerState = DirectXCommonSettings::gRasterizerDefault;
    init.mDepthStencilState = DirectXCommonSettings::gDepthDefault;
    init.mInputLayouts.resize( 2 );
    init.mInputLayouts[0].SemanticName = "POSITION";
    init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mInputLayouts[1].SemanticName = "COLOR";
    init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    std::unique_ptr<GraphicsPSO> pso = std::make_unique<GraphicsPSO>();
    if( !pso->Create( init ) )
    {
        return -1;
    }

    // 頂点を作成
    struct Vertex
    {
        Vector4 mPosition;
        Color mColor;
    };
    auto vb = std::make_unique<VertexBuffer>();
    const uint32_t kVertexCount = 3;
    if( !vb->Create( sizeof( Vertex ) * kVertexCount, sizeof( Vertex ) ) )
    {
        return -1;
    }
    Vertex v[kVertexCount] =
        {
            { Vector4( -0.3f, -0.5f, 0.0f, 1.0f ), Color::kRed },
            { Vector4( +0.0f, +0.5f, 0.0f, 1.0f ), Color::kGreen },
            { Vector4( +0.3f, -0.5f, 0.0f, 1.0f ), Color::kBlue },
        };
    vb->Update( v );

    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
    std::string path = "assets/texture/bird_fukurou_run.png";
    if( !texture->Create( path ) )
    {
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
        ImGui::Image( texture->GetSRVHdl()->mGPU.ptr, ImVec2( 256.0f, 256.0f ) );
        ImGui::End();

        editorBase.End();

        dxBase.BeginDraw();

        // 描画コマンド
        auto cmdList = dxBase.GetCmdList();
        auto windowWidth = static_cast<float>( window.GetWidth() );
        auto windowHeight = static_cast<float>( window.GetHeight() );
        cmdList->SetViewport( 0.0f, 0.0f, windowWidth, windowHeight );
        cmdList->SetScissorRect( 0.0f, 0.0f, windowWidth, windowHeight );
        rs->Bind( cmdList );
        pso->Bind( cmdList );
        cmdList->SetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        vb->Bind( dxBase.GetCmdList() );
        cmdList->DrawInstanced( kVertexCount );

        editorBase.Draw( cmdList );

        dxBase.EndDraw();
    }

    resMgr.Term();
    LOG_INFO( "Resource manager terminated." );

    editorBase.Term();
    LOG_INFO( "Editor terminated." );

    dxBase.Term();
    LOG_INFO( "DirectX terminated." );

    window.Destroy();
    LOG_INFO( "Window destroyed." );

    return 0;
}

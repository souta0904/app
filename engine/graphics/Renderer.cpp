#include "Renderer.h"

#include "core/CommandList.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "core/Window.h"
#include "utils/Logger.h"

// コンストラクタ
Renderer::Renderer()
    : mSpriteBase( nullptr )
    , mSpriteCamera( nullptr )
    , mOwlSprite( nullptr )
    , mStarSprite( nullptr )
{
}

// 初期化
bool Renderer::Init()
{
    auto& resMgr = ResourceManager::GetInstance();
    auto* spriteVS = resMgr.GetShader( "assets/shader/SpriteVS.hlsl", "vs_6_0" );
    auto* spritePS = resMgr.GetShader( "assets/shader/SpritePS.hlsl", "ps_6_0" );

    // スプライト基盤を初期化
    mSpriteBase = &SpriteBase::GetInstance();
    if( !mSpriteBase->Init( spriteVS, spritePS ) )
    {
        LOG_ERROR( "Failed to initialize sprite renderer." );
        MessageBox( nullptr, L"Failed to initialize sprite renderer.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Sprite renderer initialized successfully." );
    }

    // スプライト用カメラを初期化
    mSpriteCamera = std::make_unique<Camera>();
    mSpriteCamera->mProjectionMode = Camera::ProjectionMode::Orthographic;
    mSpriteCamera->mNearZ = 0.0f;

    // スプライト
    mOwlSprite = std::make_unique<Sprite>();
    mOwlSprite->Create( resMgr.GetTexture( "assets/texture/bird_fukurou_run.png" ) );
    mStarSprite = std::make_unique<Sprite>();
    mStarSprite->Create( resMgr.GetTexture( "assets/texture/star_pattern_transparent.png" ) );
    auto& window = Window::GetInstance();
    mStarSprite->mDrawRc.right = static_cast<float>( window.GetWidth() );
    mStarSprite->mDrawRc.bottom = static_cast<float>( window.GetHeight() );

    // ルートシグネチャを作成
    mSimpleRS = std::make_unique<RootSignature>();
    mSimpleRS->Init( 0, 0 );
    if( !mSimpleRS->Create() )
    {
        return false;
    }

    // パイプラインステートを作成
    PSOInit init = {};
    init.mRootSignature = mSimpleRS.get();
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
    mSimplePSO = std::make_unique<GraphicsPSO>();
    if( !mSimplePSO->Create( init ) )
    {
        return false;
    }

    // 頂点を作成
    struct Vertex
    {
        Vector4 mPosition;
        Color mColor;
    };
    mSimpleVB = std::make_unique<VertexBuffer>();
    if( !mSimpleVB->Create( sizeof( Vertex ) * kVertexCount, sizeof( Vertex ) ) )
    {
        return false;
    }
    Vertex v[kVertexCount] =
        {
            { Vector4( -0.3f, -0.5f, 0.0f, 1.0f ), Color::kRed },
            { Vector4( +0.0f, +0.5f, 0.0f, 1.0f ), Color::kGreen },
            { Vector4( +0.3f, -0.5f, 0.0f, 1.0f ), Color::kBlue },
        };
    mSimpleVB->Update( v );

    return true;
}

// 終了処理
void Renderer::Term()
{
    if( mSpriteBase )
    {
        mSpriteBase->Term();
        mSpriteBase = nullptr;
        LOG_INFO( "Sprite renderer terminated." );
    }
}

// 更新
void Renderer::Update()
{
    mSpriteCamera->Update();

    // UVスクロール
    mStarSprite->mUVTranslate += Vector2( 0.001f, -0.001f );
}

// 描画
void Renderer::Draw( CommandList* cmdList )
{
    auto& window = Window::GetInstance();
    auto windowWidth = static_cast<float>( window.GetWidth() );
    auto windowHeight = static_cast<float>( window.GetHeight() );
    cmdList->SetViewport( 0.0f, 0.0f, windowWidth, windowHeight );
    cmdList->SetScissorRect( 0.0f, 0.0f, windowWidth, windowHeight );

    DrawSprite( cmdList );

    cmdList->SetGraphicsRootSignature( mSimpleRS.get() );
    cmdList->SetPipelineState( mSimplePSO.get() );
    cmdList->SetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    cmdList->SetVertexBuffer( mSimpleVB.get() );
    cmdList->DrawInstanced( kVertexCount );
}

// スプライトを描画
void Renderer::DrawSprite( CommandList* cmdList )
{
    if( !mSpriteBase ) return;

    // スプライト描画開始
    mSpriteBase->Begin( cmdList );

    mStarSprite->Draw( Matrix4(), mSpriteCamera.get() );
    mOwlSprite->Draw( Matrix4(), mSpriteCamera.get() );

    // スプライト描画終了
    mSpriteBase->End();
}

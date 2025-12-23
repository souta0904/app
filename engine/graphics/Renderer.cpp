#include "Renderer.h"

#include "core/CommandList.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "core/Window.h"
#include "utils/Logger.h"

// コンストラクタ
Renderer::Renderer()
    : mSpriteBase( nullptr )
    , mModelBase( nullptr )
    , mSpriteCamera( nullptr )
    , mModelCamera( nullptr )
    , mSorter( nullptr )
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
        LOG_ERROR( "Failed to initialize sprite base." );
        MessageBox( nullptr, L"Failed to initialize sprite base.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Sprite base initialized successfully." );
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
    init.mDepthStencilState = DirectXCommonSettings::gDepthDisable;
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

    // モデル基盤を初期化
    mModelBase = &ModelBase::GetInstance();
    if( !mModelBase->Init() )
    {
        LOG_ERROR( "Failed to initialize model base." );
        MessageBox( nullptr, L"Failed to initialize model base.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Model base initialized successfully." );
    }

    mModelCamera = std::make_unique<Camera>();
    mModelCamera->mPosition = Vector3( 0.0f, 10.0f, -15.0f );

    // ソーターの初期化
    mSorter = std::make_unique<MeshSorter>();
    if( !mSorter->Initialize( mModelCamera.get() ) )
    {
        return false;
    }

    mBotModel1 = std::make_unique<ModelInstance>();
    mBotModel1->Create( resMgr.GetModel( "assets/model/bot/bot.fbx" ) );

    mBotModel2 = std::make_unique<ModelInstance>();
    mBotModel2->Create( resMgr.GetModel( "assets/model/box/box.obj" ) );

    return true;
}

// 終了処理
void Renderer::Term()
{
    if( mModelBase )
    {
        mModelBase->Term();
        mModelBase = nullptr;
        LOG_INFO( "Model base terminated." );
    }
    if( mSpriteBase )
    {
        mSpriteBase->Term();
        mSpriteBase = nullptr;
        LOG_INFO( "Sprite base terminated." );
    }
}

// 更新
void Renderer::Update()
{
    mSpriteCamera->Update();
    mModelCamera->Update();

    // UVスクロール
    mStarSprite->mUVTranslate += Vector2( 0.001f, -0.001f );

    // TODO: デルタタイムに差し替え
    mBotModel1->Update( 1.0f / 60.0f );
    mBotModel2->Update( 1.0f / 60.0f );
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

    DrawModel( cmdList );
}

// モデルを描画
void Renderer::DrawModel( CommandList* cmdList )
{
    if( !mModelBase ) return;

    mModelBase->Begin( cmdList );

    auto mat1 =
        CreateScale( Vector3::kOne * 0.1f ) *
        CreateRotate( Quaternion( Vector3::kUnitY, MathUtil::kPi ) ) *
        CreateTranslate( Vector3( -10.0f, 0.0f, 0.0f ) );
    mBotModel1->Draw( mSorter.get(), mat1 );

    auto mat2 =
        CreateScale( Vector3::kOne ) *
        CreateTranslate( Vector3( 10.0f, 0.0f, 0.0f ) );
    mBotModel2->Draw( mSorter.get(), mat2 );

    mSorter->Sort();

    mSorter->Render( cmdList );

    mModelBase->End();
}

// スプライトを描画
void Renderer::DrawSprite( CommandList* cmdList )
{
    if( !mSpriteBase ) return;

    mSpriteBase->Begin( cmdList );

    mStarSprite->Draw( Matrix4(), mSpriteCamera.get() );
    mOwlSprite->Draw( Matrix4(), mSpriteCamera.get() );

    mSpriteBase->End();
}

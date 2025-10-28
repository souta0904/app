#include "Renderer.h"

#include "core/ResourceManager.h"
#include "core/Window.h"
#include "utils/Logger.h"

// コンストラクタ
Renderer::Renderer()
    : mSpriteBase( nullptr )
    , mSpriteCamera( nullptr )
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
    mStarSprite->mDrawRc.right = static_cast<float>(window.GetWidth());
    mStarSprite->mDrawRc.bottom = static_cast<float>( window.GetHeight() );

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
    DrawSprite( cmdList );
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

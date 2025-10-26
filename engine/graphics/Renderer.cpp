#include "Renderer.h"

#include "core/ResourceManager.h"
#include "utils/Logger.h"

// コンストラクタ
Renderer::Renderer()
    : mSpriteBase( nullptr )
{
}

// 初期化
bool Renderer::Init()
{
    auto& resMgr = ResourceManager::GetInstance();
    auto* spriteVS = resMgr.GetShader( "assets/shader/SpriteVS.hlsl", "vs_6_0" );
    auto* spritePS = resMgr.GetShader( "assets/shader/SpritePS.hlsl", "ps_6_0" );

    mSpriteBase = std::make_unique<SpriteBase>();
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

    return true;
}

// 終了処理
void Renderer::Term()
{
    mSpriteBase->Term();
    LOG_INFO( "Sprite renderer terminated." );
}

// 描画
void Renderer::Draw( CommandList* cmdList )
{
    // スプライト描画開始
    mSpriteBase->Begin( cmdList );

    // スプライト描画終了
    mSpriteBase->End();
}

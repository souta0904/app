#include "LightManager.h"

#include "core/CommandList.h"
#include "math/MathUtil.h"

// コンストラクタ
LightManager::LightManager()
    : mCB( nullptr )
    , mDirectionalLights()
{
}

// 初期化
bool LightManager::Init()
{
    // 定数バッファを作成
    mCB = std::make_unique<ConstantBuffer>();
    if( !mCB->Create( sizeof( Constant ) ) )
    {
        return false;
    }

    return true;
}

// 終了処理
void LightManager::Term()
{
}

// バインド
void LightManager::Bind( CommandList* cmdList, uint32_t constIdx )
{
    if( !cmdList ) return;

    UpdateCB();

    cmdList->SetConstantBuffer( constIdx, mCB.get() );
}

// 平行光源を追加
void LightManager::AddDirectionalLight( DirectionalLight* directionalLight )
{
    if( !directionalLight ) return;

    mDirectionalLights.push_back( directionalLight );
}

// 平行光源を削除
void LightManager::RemoveDirectionalLight( DirectionalLight* directionalLight )
{
    if( !directionalLight ) return;

    auto it = std::find( mDirectionalLights.begin(), mDirectionalLights.end(), directionalLight );
    if( it != mDirectionalLights.end() )
    {
        mDirectionalLights.erase( it );
    }
}

// 定数バッファを更新
void LightManager::UpdateCB()
{
    Constant c = {};

    // 平行光源
    auto directionalLightCount = std::min<uint32_t>( kMaxDirectionalLightCount, static_cast<uint32_t>( mDirectionalLights.size() ) );
    for( uint32_t i = 0; i < directionalLightCount; ++i )
    {
        c.mDirectionalLights[i].mColor = mDirectionalLights[i]->mColor;
        c.mDirectionalLights[i].mDirection = Normalize( mDirectionalLights[i]->mDirection );
        c.mDirectionalLights[i].mIntensity = mDirectionalLights[i]->mIntensity;
    }
    c.mDirectionalLightCount = directionalLightCount;

    mCB->Update( &c );
}

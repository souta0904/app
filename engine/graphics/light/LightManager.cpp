#include "LightManager.h"

#include "core/CommandList.h"
#include "math/MathUtil.h"

// コンストラクタ
LightManager::LightManager()
    : mCB( nullptr )
    , mDirectionalLights()
    , mPointLights()
    , mSpotLights()
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

    cmdList->SetGraphicsConstantBuffer( constIdx, mCB.get() );
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

// 点光源を追加
void LightManager::AddPointLight( PointLight* pointLight )
{
    if( !pointLight ) return;

    mPointLights.push_back( pointLight );
}

// 点光源を削除
void LightManager::RemovePointLight( PointLight* pointLight )
{
    if( !pointLight ) return;

    auto it = std::find( mPointLights.begin(), mPointLights.end(), pointLight );
    if( it != mPointLights.end() )
    {
        mPointLights.erase( it );
    }
}

// スポットライトを追加
void LightManager::AddSpotLight( SpotLight* spotLight )
{
    if( !spotLight ) return;

    mSpotLights.push_back( spotLight );
}

// スポットライトを削除
void LightManager::RemoveSpotLight( SpotLight* spotLight )
{
    if( !spotLight ) return;

    auto it = std::find( mSpotLights.begin(), mSpotLights.end(), spotLight );
    if( it != mSpotLights.end() )
    {
        mSpotLights.erase( it );
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

    // 点光源
    auto pointLightCount = std::min<uint32_t>( kMaxPointLightCount, static_cast<uint32_t>( mPointLights.size() ) );
    for( uint32_t i = 0; i < pointLightCount; ++i )
    {
        c.mPointLights[i].mColor = mPointLights[i]->mColor;
        c.mPointLights[i].mPosition = mPointLights[i]->mPosition;
        c.mPointLights[i].mIntensity = mPointLights[i]->mIntensity;
        c.mPointLights[i].mRadius = mPointLights[i]->mRadius;
        c.mPointLights[i].mDecay = mPointLights[i]->mDecay;
    }

    // スポットライト
    auto spotLightCount = std::min<uint32_t>( kMaxSpotLightCount, static_cast<uint32_t>( mSpotLights.size() ) );
    for( uint32_t i = 0; i < spotLightCount; ++i )
    {
        c.mSpotLights[i].mColor = mSpotLights[i]->mColor;
        c.mSpotLights[i].mDirection = Normalize( mSpotLights[i]->mDirection );
        c.mSpotLights[i].mIntensity = mSpotLights[i]->mIntensity;
        c.mSpotLights[i].mPosition = mSpotLights[i]->mPosition;
        c.mSpotLights[i].mRadius = mSpotLights[i]->mRadius;
        c.mSpotLights[i].mDecay = mSpotLights[i]->mDecay;
        c.mSpotLights[i].mInnerCos = std::cosf( mSpotLights[i]->mInnerAngle * MathUtil::kDegToRad );
        c.mSpotLights[i].mOuterCos = std::cosf( mSpotLights[i]->mOuterAngle * MathUtil::kDegToRad );
    }

    c.mDirectionalLightCount = directionalLightCount;
    c.mPointLightCount = pointLightCount;
    c.mSpotLightCount = spotLightCount;

    mCB->Update( &c );
}

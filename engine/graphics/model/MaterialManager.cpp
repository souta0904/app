#include "MaterialManager.h"

#include "core/CommandList.h"
#include "core/DirectXBase.h"
#include "graphics/Texture.h"

MaterialManager::MaterialManager()
{

}

// 初期化
bool MaterialManager::Init()
{
    mMaterialBuff = std::make_unique<StructuredBuffer>();
    if( !mMaterialBuff->Create( kMaxMaterialCount, sizeof( MaterialData ) ) )
    {
        return false;
    }

    mSRVHdl = DirectXBase::GetInstance().CreateSRV( mMaterialBuff.get() );

    return true;
}

// 終了処理
void MaterialManager::Term()
{
}

// 登録
void MaterialManager::Register( Material* material )
{
    material->mGlobalIdx = static_cast<uint32_t>( mMaterials.size() );
    mMaterials.emplace_back( material );
}

// 解除
void MaterialManager::Unregister( Material* material )
{
    auto it = std::find( mMaterials.begin(), mMaterials.end(), material );
    if( it != mMaterials.end() )
    {
        mMaterials.erase( it );
    }

    RebuildIdx();
}

// 更新
void MaterialManager::Update()
{
    if( mMaterials.empty() ) return;

    MaterialData data[kMaxMaterialCount];
    for( uint32_t i = 0; i < mMaterials.size(); ++i )
    {
        auto m = mMaterials[i];
        m->Update();

        data[i].mColor = m->mColor;
        data[i].mIsLighting = true;
        data[i].mSpecularType = SpecularType::BlinnPhong;
        data[i].mShininess = 192.0f;
        data[i].mTextureIdx = m->GetTexture()->GetSRVHdl()->mIdx;
    }
    mMaterialBuff->Update( data );
}

// バインド
void MaterialManager::Bind( CommandList* cmdList, uint32_t srvIdx )
{
    cmdList->SetGraphicsRootDescriptorTable( srvIdx, mSRVHdl );
}

// インデックスを再構築
void MaterialManager::RebuildIdx()
{
    for( uint32_t i = 0; i < mMaterials.size(); ++i )
    {
        mMaterials[i]->mGlobalIdx = i;
    }
}

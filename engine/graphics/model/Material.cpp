#include "Material.h"

#include "core/CommandList.h"
#include "graphics/Texture.h"

// コンストラクタ
Material::Material()
    : mName( "Material" )
    , mRenderQueue( RenderQueue::Transparent )
    , mFlags( MaterialFlags::None )
    , mCB( nullptr )
    , mTexture( nullptr )
    , mColor( Color::kWhite )
{
}

// 作成
bool Material::Create( Texture* texture )
{
    SetTexture( texture );

    // 定数バッファを作成
    mCB = std::make_unique<ConstantBuffer>();
    if( !mCB->Create( sizeof( Constant ) ) )
    {
        return false;
    }

    return true;
}

// 更新
void Material::Update()
{
    Constant c = {};
    c.mColor = mColor;
    mCB->Update( &c );
}

// バインド
void Material::Bind( CommandList* cmdList, uint32_t constIdx )
{
    if( !cmdList ) return;

    cmdList->SetConstantBuffer( constIdx, mCB.get() );
}

// バインド
void Material::Bind( CommandList* cmdList, uint32_t constIdx, uint32_t texIdx )
{
    if( !cmdList || !mCB ) return;

    cmdList->SetConstantBuffer( constIdx, mCB.get() );

    // テクスチャをセット
    if( HasFlags( MaterialFlags::HasTexture ) )
    {
        cmdList->SetGraphicsRootDescriptorTable( texIdx, mTexture->GetSRVHdl() );
    }
}

// NoCullingを設定
void Material::EnableNoCulling( bool enable )
{
    enable ? AddFlags( MaterialFlags::NoCulling ) : RemoveFlags( MaterialFlags::NoCulling );
}

// Wireframeを設定
void Material::EnableWireframe( bool enable )
{
    enable ? AddFlags( MaterialFlags::Wireframe ) : RemoveFlags( MaterialFlags::Wireframe );
}

// テクスチャを設定
void Material::SetTexture( Texture* texture )
{
    mTexture = texture;
    mTexture ? AddFlags( MaterialFlags::HasTexture ) : RemoveFlags( MaterialFlags::HasTexture );
}

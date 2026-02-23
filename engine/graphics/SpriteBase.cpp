#include "SpriteBase.h"

#include "core/CommandList.h"
#include "core/DirectXCommonSettings.h"
#include "core/Window.h"

// コンストラクタ
SpriteBase::SpriteBase()
    : mRS( nullptr )
    , mPSO()
    , mCmdList( nullptr )
{
}

// 初期化
bool SpriteBase::Init( ShaderObject* vs, ShaderObject* ps )
{
    if( !vs || !ps ) return false;

    mRS = std::make_unique<RootSignature>();
    mRS->Init( 2, 1 );
    mRS->GetParameter( 0 ).InitAsCBV( 0 );
    mRS->GetParameter( 1 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 1 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 );
    mRS->GetSampler( 0 ) = DirectXCommonSettings::gSamplerLinearWrap;
    // ルートシグネチャを作成
    if( !mRS->Create() )
    {
        return false;
    }

    GraphicsPSOInit init = {};
    init.mRootSignature = mRS.get();
    init.mVS = vs;
    init.mPS = ps;
    init.mRasterizerState = DirectXCommonSettings::gRasterizerNoCulling;
    init.mDepthStencilState = DirectXCommonSettings::gDepthDisable;
    init.mInputLayouts.resize( 2 );
    init.mInputLayouts[0].SemanticName = "POSITION";
    init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mInputLayouts[1].SemanticName = "TEXCOORD";
    init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // ブレンド設定
    const std::pair<Sprite::BlendMode, D3D12_BLEND_DESC> descs[] = {
        { Sprite::BlendMode::None, DirectXCommonSettings::gBlendNone },
        { Sprite::BlendMode::Alpha, DirectXCommonSettings::gBlendAlpha },
        { Sprite::BlendMode::Add, DirectXCommonSettings::gBlendAdd },
        { Sprite::BlendMode::Subtract, DirectXCommonSettings::gBlendSubtract },
        { Sprite::BlendMode::Multiply, DirectXCommonSettings::gBlendMultiply },
        { Sprite::BlendMode::Screen, DirectXCommonSettings::gBlendScreen },
    };
    // パイプラインステートを作成
    for( auto&& [mode, desc] : descs )
    {
        init.mBlendState = desc;
        auto& pso = mPSO[static_cast<size_t>( mode )];
        pso = std::make_unique<GraphicsPSO>();
        if( !pso->Create( init ) )
        {
            return false;
        }
    }

    return true;
}

// 終了処理
void SpriteBase::Term()
{
}

// 描画開始
void SpriteBase::Begin( CommandList* cmdList, Sprite::BlendMode defaultBlend )
{
    if( !cmdList ) return;

    mCmdList = cmdList;
    mCmdList->SetGraphicsRootSignature( mRS.get() );
    SetGraphicsPSO( defaultBlend );
    mCmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
}

// 描画終了
void SpriteBase::End()
{
    mCmdList = nullptr;
}

// ブレンドモードからパイプラインを設定
void SpriteBase::SetGraphicsPSO( Sprite::BlendMode blendMode )
{
    auto& pso = mPSO[static_cast<size_t>( blendMode )];
    if( !pso ) return;

    mCmdList->SetPipelineState( pso.get() );
}

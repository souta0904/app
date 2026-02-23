#include "ModelBase.h"

#include <format>

#include "core/CommandList.h"
#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "utils/Logger.h"

// コンストラクタ
ModelBase::ModelBase()
    : mRS( nullptr )
    , mPSO()
    , mCmdList( nullptr )
{
}

// 初期化
bool ModelBase::Init()
{
    mRS = std::make_unique<RootSignature>();
    mRS->Init( 5, 1 );
    mRS->GetParameter( 0 ).InitAsCBV( 0, D3D12_SHADER_VISIBILITY_VERTEX );
    mRS->GetParameter( 1 ).InitAsCBV( 0, D3D12_SHADER_VISIBILITY_PIXEL );
    mRS->GetParameter( 2 ).InitAsCBV( 1, D3D12_SHADER_VISIBILITY_PIXEL );
    mRS->GetParameter( 3 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 3 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 );
    mRS->GetParameter( 4 ).InitAsCBV( 2, D3D12_SHADER_VISIBILITY_PIXEL );
    mRS->GetSampler( 0 ) = DirectXCommonSettings::gSamplerLinearWrap;
    // ルートシグネチャの作成
    if( !mRS->Create() )
    {
        return false;
    }

    // パイプラインステートの作成
    CreateGraphicsPSO( MakePSOKey( MeshFlags::Required, MaterialFlags::None ) );

    if( DirectXBase::kUseZPrepass )
    {
        // z-prepassの初期化
        if( !InitZPrepass() )
        {
            return false;
        }
    }

    return true;
}

// 終了処理
void ModelBase::Term()
{
}

// 描画開始
void ModelBase::Begin( CommandList* cmdList )
{
    if( !cmdList ) return;

    mCmdList = cmdList;
    mCmdList->SetGraphicsRootSignature( mRS.get() );
    mCmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

// 描画終了
void ModelBase::End()
{
    mCmdList = nullptr;
}

// パイプラインステートの設定
void ModelBase::SetGraphicsPSO( uint64_t psoKey )
{
    auto it = mPSO.find( psoKey );
    if( it == mPSO.end() )
    {
        CreateGraphicsPSO( psoKey );
        it = mPSO.find( psoKey );
        if( it == mPSO.end() ) return;
    }

    mCmdList->SetPipelineState( it->second.get() );
}

// z-prepass開始
void ModelBase::BeginZPrepass( CommandList* cmdList )
{
    if( !cmdList ) return;

    mCmdList = cmdList;
    mCmdList->SetGraphicsRootSignature( mZPrepassRS.get() );
    mCmdList->SetPipelineState( mZPrepassPSO.get() );
    mCmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

// z-prepass終了
void ModelBase::EndZPrepass()
{
    mCmdList = nullptr;
}

// パイプラインステートの作成
void ModelBase::CreateGraphicsPSO( uint64_t psoKey )
{
    auto& resMgr = ResourceManager::GetInstance();
    // auto meshFlags = GetMeshFlags( psoKey );
    auto materialFlags = GetMaterialFlags( psoKey );

    GraphicsPSOInit init = {};
    init.mRootSignature = mRS.get();

    // 頂点シェーダー
    init.mVS = resMgr.GetShader( "assets/shader/ModelVS.hlsl", "vs_6_0" );

    // ピクセルシェーダー
    if( ( materialFlags & MaterialFlags::HasTexture ) == MaterialFlags::HasTexture )
    {
        init.mPS = resMgr.GetShader( "assets/shader/ModelPS.hlsl", "ps_6_0" );
    }
    else
    {
        init.mPS = resMgr.GetShader( "assets/shader/NoTextureModelPS.hlsl", "ps_6_0" );
    }

    // ブレンド
    init.mBlendState = DirectXCommonSettings::gBlendAlpha;

    // ラスタライザ
    auto rasterizerState = DirectXCommonSettings::gRasterizerDefault;
    if( ( materialFlags & MaterialFlags::NoCulling ) == MaterialFlags::NoCulling )
    {
        rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    }
    if( ( materialFlags & MaterialFlags::Wireframe ) == MaterialFlags::Wireframe )
    {
        rasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    }
    init.mRasterizerState = rasterizerState;

    // 深度ステンシル
    if( DirectXBase::kUseZPrepass )
    {
        init.mDepthStencilState = DirectXCommonSettings::gDepthEqual;
    }
    else
    {
        init.mDepthStencilState = DirectXCommonSettings::gDepthLess;
    }

    // 頂点レイアウト
    init.mInputLayouts.resize( 3 );
    init.mInputLayouts[0].SemanticName = "POSITION";
    init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mInputLayouts[1].SemanticName = "NORMAL";
    init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mInputLayouts[2].SemanticName = "TEXCOORD";
    init.mInputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    init.mInputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    auto pso = std::make_unique<GraphicsPSO>();
    if( !pso->Create( init ) )
    {
        LOG_ERROR( std::format( "Failed to create graphics pso. Key: {}", psoKey ) );
    }

    mPSO.emplace( psoKey, std::move( pso ) );
}

// z-prepassの初期化
bool ModelBase::InitZPrepass()
{
    auto& resMgr = ResourceManager::GetInstance();

    mZPrepassRS = std::make_unique<RootSignature>();
    mZPrepassRS->Init( 1, 0 );
    mZPrepassRS->GetParameter( 0 ).InitAsCBV( 0, D3D12_SHADER_VISIBILITY_VERTEX );
    if( !mZPrepassRS->Create() )
    {
        return false;
    }

    GraphicsPSOInit init = {};
    init.mRootSignature = mZPrepassRS.get();
    init.mVS = resMgr.GetShader( "assets/shader/ZPrepassVS.hlsl", "vs_6_0" );
    init.mPS = nullptr;
    init.mBlendState = DirectXCommonSettings::gBlendNone;
    init.mRasterizerState = DirectXCommonSettings::gRasterizerDefault;
    init.mDepthStencilState = DirectXCommonSettings::gDepthLess;
    init.mInputLayouts.resize( 1 );
    init.mInputLayouts[0].SemanticName = "POSITION";
    init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mNumRenderTargets = 0;
    init.mRTVFormats[0] = DXGI_FORMAT_UNKNOWN;

    mZPrepassPSO = std::make_unique<GraphicsPSO>();
    if( !mZPrepassPSO->Create( init ) )
    {
        return false;
    }

    return true;
}

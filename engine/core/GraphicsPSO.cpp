#include "GraphicsPSO.h"

#include "DirectXBase.h"
#include "RootSignature.h"
#include "graphics/ShaderObject.h"
#include "utils/Logger.h"

// コンストラクタ
GraphicsPSO::GraphicsPSO()
    : mState( State::Unload )
    , mTask()
    , mDesc()
    , mInputLayouts()
    , mPipelineState( nullptr )
{
    // 共通の設定
    mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    mDesc.NumRenderTargets = 1;
    mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    mDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    mDesc.SampleDesc.Count = 1;
}

// コピーコンストラクタ
GraphicsPSO::GraphicsPSO( const GraphicsPSO& pso )
{
    *this = pso;
}

// 代入演算子
GraphicsPSO& GraphicsPSO::operator=( const GraphicsPSO& pso )
{
    mDesc = pso.mDesc;
    mInputLayouts = pso.mInputLayouts;
    mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
    return *this;
}

// 作成
void GraphicsPSO::Create( const PSOInit& init )
{
    mState = State::Load;

    mTask = std::async(
        std::launch::async,
        [this, init]()
        {
            try
            {
                init.mVS->GetTask().wait();
                init.mPS->GetTask().wait();
                if( !init.mVS->IsReady() || !init.mPS->IsReady() )
                {
                    throw std::runtime_error( "Failed to create graphics pipeline state." );
                }

                // 設定を移行
                mDesc.pRootSignature = init.mRootSignature->GetRootSignature().Get();
                auto blob = init.mVS->GetBlob();
                mDesc.VS.pShaderBytecode = blob->GetBufferPointer();
                mDesc.VS.BytecodeLength = blob->GetBufferSize();
                blob = init.mPS->GetBlob();
                mDesc.PS.pShaderBytecode = blob->GetBufferPointer();
                mDesc.PS.BytecodeLength = blob->GetBufferSize();
                mDesc.BlendState = init.mBlendState;
                mDesc.RasterizerState = init.mRasterizerState;
                mDesc.DepthStencilState = init.mDepthStencilState;
                mDesc.PrimitiveTopologyType = init.mPrimitiveTopologyType;

                // 頂点レイアウトを設定
                auto num = static_cast<uint32_t>( init.mInputLayouts.size() );
                mDesc.InputLayout.NumElements = num;
                if( num > 0 )
                {
                    mInputLayouts = init.mInputLayouts;
                    mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
                }

                // パイプラインステートを作成
                [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateGraphicsPipelineState( &mDesc, IID_PPV_ARGS( mPipelineState.GetAddressOf() ) );
                if( FAILED( hr ) )
                {
                    throw std::runtime_error( "Failed to create graphics pipeline state." );
                }
                mState = State::Ready;
            }
            catch( const std::exception& e )
            {
                mState = State::Error;
                LOG_ERROR( e.what() );
            }
        } );
}

// バインド
void GraphicsPSO::Bind( CommandList* cmdList )
{
    if( !cmdList ) return;

    cmdList->SetPipelineState( this );
}

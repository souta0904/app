#include "ComputePSO.h"

#include "DirectXBase.h"
#include "RootSignature.h"
#include "core/ShaderObject.h"

// コンストラクタ
ComputePSO::ComputePSO()
    : mPipelineState( nullptr )
{
}

// 作成
bool ComputePSO::Create( const ComputePSOInit& init )
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature = init.mRootSignature->GetRootSignature().Get();
    if( init.mCS )
    {
        auto blob = init.mCS->GetBlob();
        desc.CS.pShaderBytecode = blob->GetBufferPointer();
        desc.CS.BytecodeLength = blob->GetBufferSize();
    }

    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateComputePipelineState( &desc, IID_PPV_ARGS( mPipelineState.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    return true;
}

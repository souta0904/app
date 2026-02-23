#include "RootSignature.h"

#include <vector>

#include "DirectXBase.h"

// コンストラクタ
RootSignature::RootSignature()
    : mRootSignature( nullptr )
    , mNumParameters( 0 )
    , mParameters( nullptr )
    , mNumSamplers( 0 )
    , mSamplers( nullptr )
{
}

// 初期化
void RootSignature::Init( uint32_t numParameters, uint32_t numSamplers )
{
    mNumParameters = numParameters;
    mNumSamplers = numSamplers;
    if( mNumParameters > 0 )
    {
        mParameters = std::make_unique<RootParameter[]>( mNumParameters );
    }
    if( mNumSamplers > 0 )
    {
        mSamplers = std::make_unique<D3D12_STATIC_SAMPLER_DESC[]>( mNumSamplers );
    }
}

// ルートパラメータを取得
RootParameter& RootSignature::GetParameter( uint32_t idx )
{
    if( idx < 0 && idx >= mNumParameters ) nullptr;

    return mParameters[idx];
}

// サンプラーを取得
D3D12_STATIC_SAMPLER_DESC& RootSignature::GetSampler( uint32_t idx )
{
    if( idx < 0 && idx >= mNumSamplers ) nullptr;

    return mSamplers[idx];
}

// 作成
bool RootSignature::Create( D3D12_ROOT_SIGNATURE_FLAGS flags )
{
    // ルートパラメータ
    std::vector<D3D12_ROOT_PARAMETER> tmpParameters( mNumParameters );
    for( uint32_t i = 0; i < mNumParameters; ++i )
    {
        tmpParameters[i] = mParameters[i].GetRootParameter();
    }

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = mNumParameters;
    desc.pParameters = tmpParameters.data();
    desc.NumStaticSamplers = mNumSamplers;
    desc.pStaticSamplers = mSamplers.get();
    desc.Flags = flags;

    // シリアライズ
    Microsoft::WRL::ComPtr<ID3DBlob> blob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    auto hr = D3D12SerializeRootSignature( &desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errorBlob );
    if( FAILED( hr ) ) return false;

    // ルートシグネチャを作成
    hr = DirectXBase::GetInstance().GetDevice()->CreateRootSignature( 0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS( mRootSignature.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    return true;
}

#include "HiZBuilder.h"

#include <format>

#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "core/Window.h"
#include "utils/Logger.h"

// コンストラクタ
HiZBuilder::HiZBuilder()
    : mRS( nullptr )
    , mPSO( nullptr )
    , mMipLevel( 0 )
    , mHiZBuff( nullptr )
    , mSRVs()
    , mUAVs()
    , mCBs()
{
}

// 初期化
bool HiZBuilder::Init()
{
    // PSOの作成
    if( !CreatePSO() )
    {
        return false;
    }

    // ミップレベルの計算
    auto& window = Nebula::Window::GetInstance();
    auto maxSize = ( std::max )( window.GetWidth(), window.GetHeight() );
    auto log = std::log2( maxSize );  // 何回2で割れるか
    auto base = static_cast<uint32_t>( std::floor( log ) );
    mMipLevel = base + 1;  // 1x1も含めるので+1

    LOG_INFO( std::format( "HiZ MipLevel: {}", mMipLevel ).c_str() );

    // バッファとビューの作成
    if( !CreateBuff() )
    {
        return false;
    }

    return true;
}

// Hierarchical-Zを構築
void HiZBuilder::Build( CommandList* cmdList )
{
    if( !cmdList ) return;

    CopyDepth( cmdList );

    cmdList->SetComputeRootSignature( mRS.get() );
    cmdList->SetPipelineState( mPSO.get() );

    auto& window = Nebula::Window::GetInstance();
    auto windowWidth = window.GetWidth();
    auto windowHeight = window.GetHeight();

    for( uint32_t i = 0; i < mMipLevel - 1; ++i )
    {
        MipSize mipSize = {};
        mipSize.mSizeX = ( std::max )( 1u, windowWidth >> i );
        mipSize.mSizeY = ( std::max )( 1u, windowHeight >> i );
        mCBs[i]->Update( &mipSize );

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = mHiZBuff.Get();
        barrier.Transition.Subresource = i + 1;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        cmdList->ResourceBarrier( barrier );

        cmdList->SetComputeRootDescriptorTable( 0, &mSRVs[i] );
        cmdList->SetComputeRootDescriptorTable( 1, &mUAVs[i + 1] );
        cmdList->SetComputeConstantBuffer( 2, mCBs[i].get() );

        auto dstW = ( std::max )( 1u, windowWidth >> ( i + 1 ) );
        auto dstH = ( std::max )( 1u, windowHeight >> ( i + 1 ) );
        auto dispatchX = ( dstW + 7 ) / 8;
        auto dispatchY = ( dstH + 7 ) / 8;
        cmdList->Dispatch( dispatchX, dispatchY, 1 );

        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = mHiZBuff.Get();
        cmdList->ResourceBarrier( barrier );

        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = mHiZBuff.Get();
        barrier.Transition.Subresource = i + 1;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        cmdList->ResourceBarrier( barrier );
    }
}

uint32_t HiZBuilder::GetMipLevel()
{
    return mMipLevel;
}

// PSOの作成
bool HiZBuilder::CreatePSO()
{
    auto& resMgr = ResourceManager::GetInstance();

    mRS = std::make_unique<RootSignature>();
    mRS->Init( 3, 1 );
    mRS->GetParameter( 0 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 0 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 );
    mRS->GetParameter( 1 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 1 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0 );
    mRS->GetParameter( 2 ).InitAsCBV( 0 );
    mRS->GetSampler( 0 ) = DirectXCommonSettings::gSamplerLinearClamp;
    if( !mRS->Create( D3D12_ROOT_SIGNATURE_FLAG_NONE ) )
    {
        return false;
    }

    ComputePSOInit init = {};
    init.mRootSignature = mRS.get();
    init.mCS = resMgr.GetShader( "assets/shader/HiZBuildCS.hlsl", "cs_6_0" );
    mPSO = std::make_unique<ComputePSO>();
    if( !mPSO->Create( init ) )
    {
        return false;
    }

    return true;
}

// バッファとビューの作成
bool HiZBuilder::CreateBuff()
{
    auto& window = Nebula::Window::GetInstance();
    auto& dxBase = DirectXBase::GetInstance();

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = window.GetWidth();
    desc.Height = window.GetHeight();
    desc.DepthOrArraySize = 1;
    desc.MipLevels = static_cast<uint16_t>( mMipLevel );
    desc.Format = DXGI_FORMAT_R32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource(
        &DirectXCommonSettings::gHeapDefault,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS( mHiZBuff.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    mSRVs.resize( mMipLevel );
    mUAVs.resize( mMipLevel );

    for( uint32_t i = 0; i < mMipLevel; ++i )
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MostDetailedMip = i;
        srvDesc.Texture2D.MipLevels = 1;
        mSRVs[i] = dxBase.CreateSRV( srvDesc, mHiZBuff.Get() );

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = i;
        mUAVs[i] = dxBase.CreateUAV( uavDesc, mHiZBuff.Get() );
    }

    mCBs.resize( mMipLevel - 1 );
    for( uint32_t i = 0; i < mMipLevel - 1; ++i )
    {
        mCBs[i] = std::make_unique<ConstantBuffer>();
        if( !mCBs[i]->Create( sizeof( MipSize ) ) )
        {
            return false;
        }
    }

    return true;
}

// 深度をコピー
void HiZBuilder::CopyDepth( CommandList* cmdList )
{
    auto depthBuff = DirectXBase::GetInstance().GetDepthBuff();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = depthBuff.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    cmdList->ResourceBarrier( barrier );

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = mHiZBuff.Get();
    barrier.Transition.Subresource = 0;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    cmdList->ResourceBarrier( barrier );

    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource = depthBuff.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource = mHiZBuff.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    cmdList->CopyTexture( &dst, &src );

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = mHiZBuff.Get();
    barrier.Transition.Subresource = 0;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    cmdList->ResourceBarrier( barrier );

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = depthBuff.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    cmdList->ResourceBarrier( barrier );
}

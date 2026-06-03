#include "OcclusionCulling.h"

#include <format>

#include "HiZBuilder.h"
#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "core/Window.h"
#include "graphics/Camera.h"
#include "imgui/imgui.h"

// コンストラクタ
OcclusionCulling::OcclusionCulling()
    : mRS( nullptr )
    , mPSO( nullptr )
    , mHiZBuilder( nullptr )
    , mBoxBuff( nullptr )
    , mResultBuff( nullptr )
    , mCB( nullptr )
    , mBoxSRV()
    , mResultUAV()
    , mResultReadback( nullptr )
    , mResult()
    , mVisibleCount( 0 )
{
}

// 初期化
bool OcclusionCulling::Init()
{
    //// PSOの作成
    //if( !CreatePSO() )
    //{
    //    return false;
    //}

    mHiZBuilder = std::make_unique<HiZBuilder>();
    if( !mHiZBuilder->Init() )
    {
        return false;
    }

    /*mBoxBuff = std::make_unique<StructuredBuffer>();
    if( !mBoxBuff->Create( kMaxObjCount, sizeof( AABB3D ) ) )
    {
        return false;
    }

    auto& dxBase = DirectXBase::GetInstance();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = kMaxObjCount;
    srvDesc.Buffer.StructureByteStride = sizeof( AABB3D );
    mBoxSRV = dxBase.CreateSRV( srvDesc, mBoxBuff->GetResource().Get() );

    mResultBuff = std::make_unique<RWStructuredBuffer>();
    if( !mResultBuff->Create( kMaxObjCount, sizeof( uint32_t ) ) )
    {
        return false;
    }

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = kMaxObjCount;
    uavDesc.Buffer.StructureByteStride = sizeof( uint32_t );
    mResultUAV = dxBase.CreateUAV( uavDesc, mResultBuff->GetResource().Get() );

    mCB = std::make_unique<ConstantBuffer>();
    if( !mCB->Create( sizeof( Constant ) ) )
    {
        return false;
    }

    mResultReadback = std::make_unique<ReadbackBuffer>();
    if( !mResultReadback->Create( mResultBuff->GetResource() ) )
    {
        return false;
    }

    mResult.resize( kMaxObjCount );*/

    return true;
}

// 実行
void OcclusionCulling::Execute( CommandList* cmdList, const std::vector<AABB3D>& boxes, Camera* camera )
{
    auto& dxBase = DirectXBase::GetInstance();

    dxBase.ResetCmdList();
    dxBase.SetDescriptorHeap();

    mHiZBuilder->Build( cmdList );

    dxBase.WaitGPU();
    dxBase.ResetCmdList();
    dxBase.SetDescriptorHeap();

    auto& window = Nebula::Window::GetInstance();
    Constant c = {};
    c.mVP = camera->GetView() * camera->GetProjection();
    c.mScreenSize = Vector2( static_cast<float>( window.GetWidth() ), static_cast<float>( window.GetHeight() ) );
    c.mMipLevel = mHiZBuilder->mMipLevel;
    mCB->Update( &c );
    mBoxBuff->Update( boxes.data() );

    cmdList->SetComputeRootSignature( mRS.get() );
    cmdList->SetPipelineState( mPSO.get() );

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = mResultBuff->GetResource().Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    cmdList->ResourceBarrier( barrier );

    cmdList->SetComputeRootDescriptorTable( 0, &mHiZBuilder->mSRVs[0] );
    cmdList->SetComputeRootDescriptorTable( 1, &mBoxSRV );
    cmdList->SetComputeRootDescriptorTable( 2, &mResultUAV );
    cmdList->SetComputeConstantBuffer( 3, mCB.get() );

    auto groups = ( boxes.size() + 63 ) / 64;
    if( groups > 0 )
    {
        cmdList->Dispatch( static_cast<uint32_t>( groups ), 1, 1 );
    }

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource = mResultBuff->GetResource().Get();
    cmdList->ResourceBarrier( barrier );

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = mResultBuff->GetResource().Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    cmdList->ResourceBarrier( barrier );

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = mResultReadback->GetResource().Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    cmdList->ResourceBarrier( barrier );

    cmdList->CopyResource( mResultReadback->GetResource().Get(), mResultBuff->GetResource().Get() );

    dxBase.WaitGPU();

    mVisibleCount = 0;
    auto result = static_cast<uint32_t*>( mResultReadback->Map() );
    for( uint32_t i = 0; i < kMaxObjCount; ++i )
    {
        mResult[i] = result[i] != 0;
        if( result[i] != 0 ) ++mVisibleCount;
    }
    mResultReadback->Unmap();
}

// PSOの作成
bool OcclusionCulling::CreatePSO()
{
    auto& resMgr = ResourceManager::GetInstance();

    mRS = std::make_unique<RootSignature>();
    mRS->Init( 4, 1 );
    mRS->GetParameter( 0 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 0 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 );
    mRS->GetParameter( 1 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 1 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1 );
    mRS->GetParameter( 2 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 2 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0 );
    mRS->GetParameter( 3 ).InitAsCBV( 0 );
    mRS->GetSampler( 0 ) = DirectXCommonSettings::gSamplerLinearClamp;
    if( !mRS->Create( D3D12_ROOT_SIGNATURE_FLAG_NONE ) )
    {
        return false;
    }

    ComputePSOInit init = {};
    init.mRootSignature = mRS.get();
    init.mCS = resMgr.GetShader( "assets/shader/OcclusionCulling.hlsl", "cs_6_0" );
    mPSO = std::make_unique<ComputePSO>();
    if( !mPSO->Create( init ) )
    {
        return false;
    }

    return true;
}

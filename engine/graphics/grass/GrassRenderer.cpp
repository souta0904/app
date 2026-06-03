#include "GrassRenderer.h"

#include <format>

#include "core/CommandList.h"
#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "graphics/Camera.h"
#include "math/Random.h"
#include "utils/Logger.h"
#include "core/Window.h"
#include "graphics/hiz/HiZBuilder.h"

GrassRenderer::GrassRenderer()
{
}

bool GrassRenderer::Init()
{
    // インダイレクト描画
    mIndirectRS = std::make_unique<RootSignature>();
    mIndirectRS->Init( 5, 1 );
    mIndirectRS->GetParameter( 0 ).InitAsDescriptorTable( 1 );
    mIndirectRS->GetParameter( 0 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0 );
    mIndirectRS->GetParameter( 1 ).InitAsConstants( 0, 2 );
    mIndirectRS->GetParameter( 2 ).InitAsCBV( 1 );
    mIndirectRS->GetParameter( 3 ).InitAsDescriptorTable( 1 );
    mIndirectRS->GetParameter( 3 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 );
    mIndirectRS->GetParameter( 4 ).InitAsDescriptorTable( 1 );
    mIndirectRS->GetParameter( 4 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1 );
    mIndirectRS->GetSampler( 0 ) = DirectXCommonSettings::gSamplerLinearClamp;
    if( !mIndirectRS->Create() )
    {
        return false;
    }

    auto& resMgr = ResourceManager::GetInstance();
    ComputePSOInit computeInit = {};
    computeInit.mRootSignature = mIndirectRS.get();
    computeInit.mCS = resMgr.GetShader( "assets/shader/IndirectCS.hlsl", "cs_6_0" );
    mIndirectPSO = std::make_unique<ComputePSO>();
    if( !mIndirectPSO->Create( computeInit ) )
    {
        return false;
    }

    // 描画
    mRS = std::make_unique<RootSignature>();
    mRS->Init( 3, 0 );
    mRS->GetParameter( 0 ).InitAsDescriptorTable( 1 );
    mRS->GetParameter( 0 ).SetDescriptorRange( 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 );
    mRS->GetParameter( 1 ).InitAsConstants( 0, 1 );
    mRS->GetParameter( 2 ).InitAsCBV( 1 );
    if( !mRS->Create() )
    {
        return false;
    }

    GraphicsPSOInit graphicsInit = {};
    graphicsInit.mRootSignature = mRS.get();
    graphicsInit.mVS = resMgr.GetShader( "assets/shader/GrassVS.hlsl", "vs_6_0" );
    graphicsInit.mPS = resMgr.GetShader( "assets/shader/GrassPS.hlsl", "ps_6_0" );
    graphicsInit.mBlendState = DirectXCommonSettings::gBlendAlpha;
    auto rasterizerState = DirectXCommonSettings::gRasterizerDefault;
    rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    graphicsInit.mRasterizerState = rasterizerState;
    graphicsInit.mDepthStencilState = DirectXCommonSettings::gDepthLess;
    graphicsInit.mInputLayouts.resize( 3 );
    graphicsInit.mInputLayouts[0].SemanticName = "POSITION";
    graphicsInit.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    graphicsInit.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    graphicsInit.mInputLayouts[1].SemanticName = "NORMAL";
    graphicsInit.mInputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    graphicsInit.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    graphicsInit.mInputLayouts[2].SemanticName = "TEXCOORD";
    graphicsInit.mInputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    graphicsInit.mInputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    mPSO = std::make_unique<GraphicsPSO>();
    if( !mPSO->Create( graphicsInit ) )
    {
        return false;
    }

    D3D12_INDIRECT_ARGUMENT_DESC indirectArgs[2] = {};
    indirectArgs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
    indirectArgs[0].Constant.RootParameterIndex = 1;
    indirectArgs[0].Constant.Num32BitValuesToSet = 1;
    indirectArgs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
    D3D12_COMMAND_SIGNATURE_DESC desc = {};
    desc.ByteStride = sizeof( IndirectCommand );
    desc.NumArgumentDescs = _countof( indirectArgs );
    desc.pArgumentDescs = indirectArgs;
    auto device = DirectXBase::GetInstance().GetDevice();
    auto hr = device->CreateCommandSignature( &desc, mRS->GetRootSignature().Get(), IID_PPV_ARGS( mCommandSignature.GetAddressOf() ) );
    if( FAILED( hr ) )
    {
        return false;
    }

    mIndirectBuff = std::make_unique<RWStructuredBuffer>();
    if( !mIndirectBuff->Create( kMax, sizeof( IndirectCommand ), true ) )
    {
        return false;
    }

    // カウンタリセット用
    D3D12_RESOURCE_DESC counterDesc = {};
    counterDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    counterDesc.Width = sizeof( uint32_t );
    counterDesc.Height = 1;
    counterDesc.DepthOrArraySize = 1;
    counterDesc.MipLevels = 1;
    counterDesc.SampleDesc.Count = 1;
    counterDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    hr = device->CreateCommittedResource(
        &DirectXCommonSettings::gHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &counterDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS( mCounterResetBuff.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;
    void* ptr = nullptr;
    mCounterResetBuff->Map( 0, nullptr, &ptr );
    uint32_t zero = 0;
    memcpy( ptr, &zero, sizeof( uint32_t ) );
    mCounterResetBuff->Unmap( 0, nullptr );

    mTransformationBuff = std::make_unique<StructuredBuffer>();
    mTransformationBuff->Create( kMax, sizeof( TransformationMatrix ) );

    mSceneBuff = std::make_unique<ConstantBuffer>();
    mSceneBuff->Create( sizeof( Scene ) );

    mUAVHdl = DirectXBase::GetInstance().CreateUAV( mIndirectBuff.get() );
    mSRVHdl = DirectXBase::GetInstance().CreateSRV( mTransformationBuff.get() );

    mModel = resMgr.GetModel( "assets/model/grass/grass.obj" );
    InstanceData data[kMax];
    auto mesh = mModel->GetMesh();
    for( uint32_t x = 0; x < 100; ++x )
    {
        for( uint32_t z = 0; z < 100; ++z )
        {
            mRotate[x * 100 + z] = Random::Next( 0, 360 ) * MathUtil::kDegToRad;
            Vector3 pos = Vector3( ( x - 50.0f ) * 5.0f, 0.0f, ( z - 50.0f ) * 5.0f ) / 2.0f + Random::Next(Vector3(-1.0f,0.0f,-1.0f),Vector3(1.0f,0.0f,1.0f));
            mat[x * 100 + z].mWorld =
                CreateScale( Vector3::kOne * 3.0f ) *
                CreateRotateY( mRotate[x * 100 + z] ) *
                CreateTranslate( pos );

            auto aabb = mesh->GetAABB();
            auto w = GetWorldAABB( aabb, mat[x * 100 + z].mWorld );
            data[x * 100 + z].mMin = w.mMin;
            data[x * 100 + z].mMax = w.mMax;
        }
    }
    mTransformationBuff->Update( mat );

    mCameraBuff = std::make_unique<ConstantBuffer>();
    mCameraBuff->Create( sizeof( Frustum ) );

    mInstanceDataBuff = std::make_unique<StructuredBuffer>();
    mInstanceDataBuff->Create( kMax, sizeof( InstanceData ) );
    mInstanceDataBuff->Update( data );
    mInstanceSRVHdl = DirectXBase::GetInstance().CreateSRV( mInstanceDataBuff.get() );

    return true;
}

void GrassRenderer::Term()
{
}

void GrassRenderer::Update( float deltaTime )
{
    mScene.mTime += deltaTime;
    mSceneBuff->Update( &mScene );
}

void GrassRenderer::Begin( CommandList* cmdList )
{
    if( !cmdList ) return;

    mCmdList = cmdList;
}

void GrassRenderer::End()
{
    mCmdList = nullptr;
}

void GrassRenderer::Draw( Camera* camera, Camera* fCamera,HiZBuilder* hiz )
{
    Matrix4 vp = camera->GetView() *
                 camera->GetProjection();
    mScene.mVP = vp;



    mSceneBuff->Update( &mScene );

    Frustum f = {};
    for( int i = 0; i < 6; ++i )
    {
        f.mPlanes[i].x = fCamera->GetFrustum().mPlanes[i].mNormal.x;
        f.mPlanes[i].y = fCamera->GetFrustum().mPlanes[i].mNormal.y;
        f.mPlanes[i].z = fCamera->GetFrustum().mPlanes[i].mNormal.z;
        f.mPlanes[i].w = fCamera->GetFrustum().mPlanes[i].mD;
    }
    auto& window = Nebula::Window::GetInstance();
    f.mVP = fCamera->GetView() * fCamera->GetProjection();
    f.mScreenSize = Vector2( static_cast<float>( window.GetWidth() ), static_cast<float>( window.GetHeight() ) );
    f.mMipLevel = hiz->GetMipLevel();
    mCameraBuff->Update( &f );



    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Transition.pResource = mIndirectBuff->GetCounterResource().Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    mCmdList->ResourceBarrier( barrier );

    // クリア
    mCmdList->GetCmdList()->CopyBufferRegion(
        mIndirectBuff->GetCounterResource().Get(),
        0,
        mCounterResetBuff.Get(),
        0,
        sizeof( uint32_t ) );

    mCmdList->SetComputeRootSignature( mIndirectRS.get() );
    mCmdList->SetPipelineState( mIndirectPSO.get() );

    mCmdList->SetComputeRootDescriptorTable( 0, mUAVHdl );
    mCmdList->SetComputeConstantBuffer( 2, mCameraBuff.get() );
    mCmdList->SetComputeRootDescriptorTable( 3, mInstanceSRVHdl );
    auto a = hiz->GetSRV();
    mCmdList->SetComputeRootDescriptorTable( 4, &a );

    auto mesh = mModel->GetMesh();
    struct
    {
        uint32_t mInstanceCount;
        uint32_t mIndexCount;
    } params;
    params.mInstanceCount = kMax;
    params.mIndexCount = mesh->GetIndicesCount();
    mCmdList->GetCmdList()->SetComputeRoot32BitConstants( 1, 2, &params, 0 );

    mCmdList->Dispatch( ( kMax + 63 ) / 64, 1, 1 );

    D3D12_RESOURCE_BARRIER barriers[2] = {};
    barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barriers[0].UAV.pResource = mIndirectBuff->GetResource().Get();
    mCmdList->ResourceBarrier( barriers[0] );
    barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barriers[1].UAV.pResource = mIndirectBuff->GetCounterResource().Get();
    mCmdList->ResourceBarrier( barriers[1] );

    mCmdList->SetGraphicsRootSignature( mRS.get() );
    mCmdList->SetPipelineState( mPSO.get() );
    mCmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    mCmdList->SetGraphicsRootDescriptorTable( 0, mSRVHdl );
    mCmdList->SetGraphicsConstantBuffer( 2, mSceneBuff.get() );

    mCmdList->SetVertexBuffer( mesh->GetVertexBuffer() );
    mCmdList->SetIndexBuffer( mesh->GetIndexBuffer() );

    barrier.Transition.pResource = mIndirectBuff->GetCounterResource().Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    mCmdList->ResourceBarrier( barrier );

    mCmdList->ExecuteIndirect(
        mCommandSignature.Get(),
        kMax,
        mIndirectBuff->GetResource().Get(),
        0,
        mIndirectBuff->GetCounterResource().Get(),
        0 );

    barrier.Transition.pResource = mIndirectBuff->GetCounterResource().Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
    mCmdList->ResourceBarrier( barrier );
}

AABB3D GrassRenderer::GetWorldAABB( const AABB3D& local, const Matrix4& world )
{
    AABB3D worldAABB;
    worldAABB.Reset();

    auto min = local.mMin;
    auto max = local.mMax;

    Vector3 v[8] = {};
    v[0] = min;
    v[1] = Vector3( max.x, min.y, min.z );
    v[2] = Vector3( max.x, min.y, max.z );
    v[3] = Vector3( min.x, min.y, max.z );
    v[4] = Vector3( min.x, max.y, min.z );
    v[5] = Vector3( max.x, max.y, min.z );
    v[6] = max;
    v[7] = Vector3( min.x, max.y, max.z );

    for( uint32_t j = 0; j < 8; ++j )
    {
        auto p = v[j] * world;
        worldAABB.Update( p );
    }

    return worldAABB;
}

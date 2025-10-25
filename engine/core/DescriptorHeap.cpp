#include "DescriptorHeap.h"

#include "DirectXBase.h"

// コンストラクタ
DescriptorHeap::DescriptorHeap()
    : mDescriptorHeap( nullptr )
    , mDescriptorHdlPool()
    , mIncrementSize( 0 )
    , mIsShaderVisible( false )
{
}

// 作成
bool DescriptorHeap::Create( Type type, uint32_t numDescriptors, bool isShaderVisible )
{
    mIsShaderVisible = isShaderVisible;

    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    switch( type )
    {
        case Type::CBV_SRV_UAV:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            break;
        case Type::SAMPLER:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            break;
        case Type::RTV:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            break;
        case Type::DSV:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            break;
        default:
            return false;
    }
    desc.NumDescriptors = numDescriptors;
    desc.Flags = mIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // デスクリプタヒープを作成
    auto hr = device->CreateDescriptorHeap( &desc, IID_PPV_ARGS( mDescriptorHeap.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // デスクリプタハンドルのリストを作成
    mDescriptorHdlPool = std::make_unique<SimplePool<DescriptorHandle>>();
    mDescriptorHdlPool->Init( desc.NumDescriptors );

    // デスクリプタのインクリメントサイズを取得
    mIncrementSize = device->GetDescriptorHandleIncrementSize( desc.Type );

    return true;
}

// デスクリプタハンドルを割り当て
DescriptorHandle* DescriptorHeap::Alloc()
{
    if( !mDescriptorHeap ) return nullptr;

    auto* hdl = mDescriptorHdlPool->Lend();
    if( hdl )
    {
        InitHdl( hdl->mIdx, hdl->mValue );
        return &hdl->mValue;
    }
    return nullptr;
}

// デスクリプタハンドルを解放
void DescriptorHeap::Free( DescriptorHandle*& hdl )
{
    if( hdl )
    {
        mDescriptorHdlPool->Return( hdl->mIdx );
        hdl = nullptr;
    }
}

// デスクリプタハンドルを初期化
void DescriptorHeap::InitHdl( uint32_t idx, DescriptorHandle& initHdl )
{
    initHdl.mCPU = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    initHdl.mCPU.ptr += mIncrementSize * idx;
    if( mIsShaderVisible )
    {
        initHdl.mGPU = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        initHdl.mGPU.ptr += mIncrementSize * idx;
    }
    initHdl.mIdx = idx;
}

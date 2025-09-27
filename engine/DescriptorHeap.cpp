#include "DescriptorHeap.h"

// コンストラクタ
DescriptorHeap::DescriptorHeap()
    : mDescriptorHeap( nullptr )
    , mDescriptorHdls()
    , mIncrementSize( 0 )
    , mIsShaderVisible( false )
{
}

// デスクリプタヒープを作成
bool DescriptorHeap::Create( Type type, uint32_t numDescriptors, bool isShaderVisible )
{
    mIsShaderVisible = isShaderVisible;

    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    switch( type )
    {
        case Type::kCBV_SRV_UAV:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            break;
        case Type::kSAMPLER:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            break;
        case Type::kRTV:
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            break;
        case Type::kDSV:
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

    // デスクリプタハンドルリストを作成
    mDescriptorHdls.clear();
    mDescriptorHdls.resize( desc.NumDescriptors );
    mNextIndex = 0;

    // インクリメントサイズを取得
    mIncrementSize = device->GetDescriptorHandleIncrementSize( desc.Type );

    return true;
}

// デスクリプタヒープをバインド
void DescriptorHeap::Bind( CommandList* cmdList )
{
    // TODO: 実装
}

// デスクリプタハンドルを割り当て
DescriptorHandle* DescriptorHeap::Alloc()
{
    // リングバッファで空きを探す
    auto size = static_cast<uint32_t>( mDescriptorHdls.size() );
    for( uint32_t i = 0; i < size; ++i )
    {
        uint32_t idx = ( mNextIndex + i ) % size;
        if( !mDescriptorHdls[idx].mIsActive )
        {
            InitHdl( idx, mDescriptorHdls[idx] );
            mNextIndex = ( idx + 1 ) % size;
            return &mDescriptorHdls[idx];
        }
    }
    return nullptr;  // 空きがない
}

// デスクリプタハンドルを解放
void DescriptorHeap::Free( DescriptorHandle*& hdl )
{
    if( hdl )
    {
        mDescriptorHdls[hdl->mIndex].mIsActive = false;
        hdl = nullptr;
    }
}

// デスクリプタハンドルを初期化
void DescriptorHeap::InitHdl( uint32_t index, DescriptorHandle& initHdl )
{
    initHdl.mCPU = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    initHdl.mCPU.ptr += mIncrementSize * index;
    if( mIsShaderVisible )
    {
        initHdl.mGPU = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        initHdl.mGPU.ptr += mIncrementSize * index;
    }
    initHdl.mIsActive = true;
    initHdl.mIndex = index;
}

#include "StructuredBuffer.h"

#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"

// コンストラクタ
StructuredBuffer::StructuredBuffer()
    : mResource( nullptr )
    , mCount( 0 )
    , mStrideSize( 0 )
    , mSize( 0 )
    , mData( nullptr )
{
}

// デストラクタ
StructuredBuffer::~StructuredBuffer()
{
    if( mResource && mData )
    {
        mResource->Unmap( 0, nullptr );
        mData = nullptr;
    }
}

// 作成
bool StructuredBuffer::Create( uint32_t count, uint32_t strideSize )
{
    mCount = count;
    mStrideSize = strideSize;
    mSize = count * strideSize;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = ( mSize + 0xff ) & ~0xff;  // アライメント
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 構造化バッファを作成
    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource(
        &DirectXCommonSettings::gHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // 仮想アドレスをマッピング
    // マップしっぱなし
    mResource->Map( 0, nullptr, &mData );

    return true;
}

// 更新
void StructuredBuffer::Update( const void* data )
{
    if( !mData ) return;

    memcpy( mData, data, mSize );
}

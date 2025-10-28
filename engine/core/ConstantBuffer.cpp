#include "ConstantBuffer.h"

#include "DirectXBase.h"
#include "DirectXCommonSettings.h"

// コンストラクタ
ConstantBuffer::ConstantBuffer()
    : mResource( nullptr )
    , mSize( 0 )
    , mData( nullptr )
{
}

// 作成
bool ConstantBuffer::Create( uint32_t size )
{
    mSize = size;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = ( mSize + 0xff ) & ~0xff;  // アライメント
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 定数バッファを作成
    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource(
        &DirectXCommonSettings::gHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // 仮想アドレスをマッピング
    mResource->Map( 0, nullptr, &mData );

    return true;
}

// 更新
void ConstantBuffer::Update( void* data )
{
    if( !mData ) return;

    memcpy( mData, data, mSize );
}

#include "IndexBuffer.h"

#include "DirectXBase.h"
#include "DirectXCommonSettings.h"

// コンストラクタ
IndexBuffer::IndexBuffer()
    : mResource( nullptr )
    , mView()
    , mData( nullptr )
{
}

// 作成
bool IndexBuffer::Create( uint32_t size )
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // インデックスバッファを作成
    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource(
        &DirectXCommonSettings::gHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // インデックスバッファビュー
    mView.BufferLocation = mResource->GetGPUVirtualAddress();
    mView.SizeInBytes = size;
    mView.Format = DXGI_FORMAT_R32_UINT;

    // 仮想アドレスをマッピング
    mResource->Map( 0, nullptr, &mData );

    return true;
}

// 更新
void IndexBuffer::Update( void* data )
{
    if( !mData ) return;

    memcpy( mData, data, mView.SizeInBytes );
}

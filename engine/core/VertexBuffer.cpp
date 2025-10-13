#include "VertexBuffer.h"

#include "DirectXBase.h"
#include "DirectXCommonSettings.h"

VertexBuffer::VertexBuffer()
    : mResource( nullptr )
    , mView()
    , mData( nullptr )
{
}

// 作成
bool VertexBuffer::Create( uint32_t size, uint32_t strideSize )
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 頂点バッファを作成
    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource( &DirectXCommonSettings::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // 頂点バッファビュー
    mView.BufferLocation = mResource->GetGPUVirtualAddress();
    mView.SizeInBytes = size;
    mView.StrideInBytes = strideSize;

    // 仮想アドレスをマッピング
    mResource->Map( 0, nullptr, &mData );

    return true;
}

// 更新
void VertexBuffer::Update( void* data )
{
    if( !mData ) return;

    memcpy( mData, data, mView.SizeInBytes );
}

// バインド
void VertexBuffer::Bind( CommandList* cmdList )
{
    if( !cmdList ) return;

    cmdList->SetVertexBuffer( mView );
}

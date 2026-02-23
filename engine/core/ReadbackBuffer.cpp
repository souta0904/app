#include "ReadbackBuffer.h"

#include "DirectXBase.h"
#include "DirectXCommonSettings.h"

// コンストラクタ
ReadbackBuffer::ReadbackBuffer()
    : mResource( nullptr )
    , mSize( 0 )
    , mData( nullptr )
{
}

// 作成
bool ReadbackBuffer::Create( Microsoft::WRL::ComPtr<ID3D12Resource> srcBuff )
{
    auto srcDesc = srcBuff->GetDesc();
    mSize = static_cast<uint32_t>( srcDesc.Width );

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = mSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 読み戻しバッファを作成
    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource(
        &DirectXCommonSettings::gHeapReadback,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    return true;
}

// マップ
void* ReadbackBuffer::Map()
{
    mResource->Map( 0, nullptr, &mData );
    return mData;
}

// アンマップ
void ReadbackBuffer::Unmap()
{
    if( mResource && mData )
    {
        mResource->Unmap( 0, nullptr );
        mData = nullptr;
    }
}

// コピー
void ReadbackBuffer::CopyFrom( CommandList* cmdList, Microsoft::WRL::ComPtr<ID3D12Resource> srcBuff )
{
    if( !cmdList ) return;

    cmdList->CopyResource( mResource.Get(), srcBuff.Get() );
}

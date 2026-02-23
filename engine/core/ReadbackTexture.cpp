#include "ReadbackTexture.h"

#include "DirectXBase.h"
#include "DirectXCommonSettings.h"

// コンストラクタ
ReadbackTexture::ReadbackTexture()
    : mResource( nullptr )
    , mFootprint()
    , mSize()
    , mData( nullptr )
{
}

// 作成
bool ReadbackTexture::Create( Microsoft::WRL::ComPtr<ID3D12Resource> srcBuff )
{
    auto device = DirectXBase::GetInstance().GetDevice();

    auto srcDesc = srcBuff->GetDesc();
    device->GetCopyableFootprints( &srcDesc, 0, 1, 0, &mFootprint, nullptr, nullptr, &mSize );

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
void* ReadbackTexture::Map()
{
    mResource->Map( 0, nullptr, &mData );
    return mData;
}

// アンマップ
void ReadbackTexture::Unmap()
{
    if( mResource && mData )
    {
        mResource->Unmap( 0, nullptr );
        mData = nullptr;
    }
}

// コピー
void ReadbackTexture::CopyFrom( CommandList* cmdList, Microsoft::WRL::ComPtr<ID3D12Resource> srcBuff )
{
    if( !cmdList ) return;

    D3D12_TEXTURE_COPY_LOCATION dstLoc = {};
    dstLoc.pResource = mResource.Get();
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLoc.PlacedFootprint = mFootprint;

    D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
    srcLoc.pResource = srcBuff.Get();
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLoc.SubresourceIndex = 0;

    cmdList->CopyTexture( &dstLoc, &srcLoc );
}

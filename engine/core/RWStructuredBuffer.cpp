#include "RWStructuredBuffer.h"

#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"

// コンストラクタ
RWStructuredBuffer::RWStructuredBuffer()
    : mResource( nullptr )
    , mCount( 0 )
    , mStrideSize( 0 )
    , mSize( 0 )
    , mData( nullptr )
{
}

// 作成
bool RWStructuredBuffer::Create( uint32_t count, uint32_t strideSize, bool enableCounter )
{
    mCount = count;
    mStrideSize = strideSize;
    mSize = count * strideSize;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = ( mSize + 0xff ) & ~0xff;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    // 読み取り/書き込みバッファを作成
    auto device = DirectXBase::GetInstance().GetDevice();
    [[maybe_unused]] auto hr = device->CreateCommittedResource(
        &DirectXCommonSettings::gHeapDefault,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // カウンタ生成
    if( enableCounter )
    {
        D3D12_RESOURCE_DESC counterDesc = {};
        counterDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        counterDesc.Width = sizeof( uint32_t );
        counterDesc.Height = 1;
        counterDesc.DepthOrArraySize = 1;
        counterDesc.MipLevels = 1;
        counterDesc.SampleDesc.Count = 1;
        counterDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        counterDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        hr = device->CreateCommittedResource(
            &DirectXCommonSettings::gHeapDefault,
            D3D12_HEAP_FLAG_NONE,
            &counterDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS( mCounterResource.GetAddressOf() ) );
        if( FAILED( hr ) ) return false;
    }

    return true;
}

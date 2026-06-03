#include "DepthBuffer.h"

#include "core/DescriptorHandle.h"
#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"

namespace Nebula
{

DepthBuffer::DepthBuffer()
    : GPUResource()
    , mDSVHdl( nullptr )
{
}

bool DepthBuffer::Create( DirectXBase* dxBase, uint32_t width, uint32_t height )
{
    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resDesc.Width = width;
    resDesc.Height = height;
    resDesc.DepthOrArraySize = 1;
    resDesc.Format = DXGI_FORMAT_D32_FLOAT;
    resDesc.SampleDesc.Count = 1;
    resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearVal = {};
    clearVal.Format = DXGI_FORMAT_D32_FLOAT;
    clearVal.DepthStencil.Depth = 1.0f;

    // 深度バッファの作成
    auto device = dxBase->GetDevice();
    [[maybe_unused]] auto hr = device->CreateCommittedResource( &DirectXCommonSettings::gHeapDefault, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal, IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    mDSVHdl = dxBase->GetDSVHeap()->Alloc();
    if( !mDSVHdl ) return false;

    // 深度ステンシルビューの作成
    D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
    viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView( mResource.Get(), &viewDesc, mDSVHdl->mCPU );

    return true;
}

}  // namespace Nebula

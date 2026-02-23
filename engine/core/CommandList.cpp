#include "CommandList.h"

#include "ComputePSO.h"
#include "ConstantBuffer.h"
#include "DirectXBase.h"
#include "GraphicsPSO.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "graphics/Texture.h"

D3D12_COMMAND_LIST_TYPE CommandList::GetCommandListType( Type type )
{
    switch( type )
    {
        case Type::Direct:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case Type::Copy:
            return D3D12_COMMAND_LIST_TYPE_COPY;
        default:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

// コンストラクタ
CommandList::CommandList()
    : mCmdAllocators()
    , mCmdList( nullptr )
{
}

// 作成
bool CommandList::Create( Type type, uint32_t count )
{
    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    auto cmdListType = CommandList::GetCommandListType( type );

    mCmdAllocators.resize( count );
    for( auto& cmdAllocator : mCmdAllocators )
    {
        // コマンドアロケーターを作成
        auto hr = device->CreateCommandAllocator( cmdListType, IID_PPV_ARGS( cmdAllocator.GetAddressOf() ) );
        if( FAILED( hr ) ) return false;
    }

    // コマンドリストを作成
    auto hr = device->CreateCommandList( 0, cmdListType, mCmdAllocators[0].Get(), nullptr, IID_PPV_ARGS( mCmdList.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // 閉じておく
    mCmdList->Close();

    return true;
}

// クローズ
void CommandList::Close()
{
    if( !mCmdList ) return;

    mCmdList->Close();
}

// リセット
void CommandList::Reset( uint32_t idx )
{
    if( idx >= mCmdAllocators.size() ) return;

    if( !mCmdAllocators[idx] || !mCmdList ) return;

    mCmdAllocators[idx]->Reset();
    mCmdList->Reset( mCmdAllocators[idx].Get(), nullptr );
}

#pragma region ID3D12GraphicsCommandListラッパー

// 深度バッファをクリア
void CommandList::ClearDepthStencilView( DescriptorHandle* hDSV )
{
    if( !mCmdList || !hDSV ) return;

    mCmdList->ClearDepthStencilView( hDSV->mCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );
}

// レンダーターゲットをクリア
void CommandList::ClearRenderTargetView( DescriptorHandle* hRTV, const float clearColor[4] )
{
    if( !mCmdList || !hRTV ) return;

    mCmdList->ClearRenderTargetView( hRTV->mCPU, clearColor, 0, nullptr );
}

// リソースをコピー
void CommandList::CopyResource( ID3D12Resource* dst, ID3D12Resource* src )
{
    if( !mCmdList || !dst || !src ) return;

    mCmdList->CopyResource( dst, src );
}

// テクスチャをコピー
void CommandList::CopyTexture( const D3D12_TEXTURE_COPY_LOCATION* dst, const D3D12_TEXTURE_COPY_LOCATION* src )
{
    if( !mCmdList || !dst || !src ) return;

    mCmdList->CopyTextureRegion( dst, 0, 0, 0, src, nullptr );
}

// コンピュートシェーダを実行
void CommandList::Dispatch( uint32_t x, uint32_t y, uint32_t z )
{
    if( !mCmdList ) return;

    mCmdList->Dispatch( x, y, z );
}

// 描画
void CommandList::DrawInstanced( uint32_t vertexCount )
{
    if( !mCmdList ) return;

    mCmdList->DrawInstanced( vertexCount, 1, 0, 0 );
}

// 描画
void CommandList::DrawIndexedInstanced( uint32_t indexCount )
{
    if( !mCmdList ) return;

    mCmdList->DrawIndexedInstanced( indexCount, 1, 0, 0, 0 );
}

// リソースバリアをセット
void CommandList::ResourceBarrier( const D3D12_RESOURCE_BARRIER& barrier )
{
    if( !mCmdList ) return;

    mCmdList->ResourceBarrier( 1, &barrier );
}

// 定数バッファをセット
void CommandList::SetComputeConstantBuffer( uint32_t rootParamIdx, ConstantBuffer* constantBuffer )
{
    if( !mCmdList || !constantBuffer ) return;

    mCmdList->SetComputeRootConstantBufferView( rootParamIdx, constantBuffer->GetGPUVirtualAddress() );
}

// 定数バッファをセット
void CommandList::SetGraphicsConstantBuffer( uint32_t rootParamIdx, ConstantBuffer* constantBuffer )
{
    if( !mCmdList || !constantBuffer ) return;

    mCmdList->SetGraphicsRootConstantBufferView( rootParamIdx, constantBuffer->GetGPUVirtualAddress() );
}

// デスクリプタヒープをセット
void CommandList::SetDescriptorHeap( DescriptorHeap* descriptorHeap )
{
    if( !mCmdList || !descriptorHeap ) return;

    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap->GetDescriptorHeap().Get() };
    mCmdList->SetDescriptorHeaps( 1, descriptorHeaps );
}

// デスクリプタテーブルをルートシグネチャへセット
void CommandList::SetComputeRootDescriptorTable( uint32_t rootParamIdx, DescriptorHandle* descriptorHandle )
{
    if( !mCmdList || !descriptorHandle ) return;

    mCmdList->SetComputeRootDescriptorTable( rootParamIdx, descriptorHandle->mGPU );
}

// デスクリプタテーブルをルートシグネチャへセット
void CommandList::SetGraphicsRootDescriptorTable( uint32_t rootParamIdx, DescriptorHandle* descriptorHandle )
{
    if( !mCmdList || !descriptorHandle ) return;

    mCmdList->SetGraphicsRootDescriptorTable( rootParamIdx, descriptorHandle->mGPU );
}

// ルートシグネチャをセット
void CommandList::SetComputeRootSignature( RootSignature* rootSignature )
{
    if( !mCmdList || !rootSignature ) return;

    mCmdList->SetComputeRootSignature( rootSignature->GetRootSignature().Get() );
}

// ルートシグネチャをセット
void CommandList::SetGraphicsRootSignature( RootSignature* rootSignature )
{
    if( !mCmdList || !rootSignature ) return;

    mCmdList->SetGraphicsRootSignature( rootSignature->GetRootSignature().Get() );
}

// インデックスバッファをセット
void CommandList::SetIndexBuffer( IndexBuffer* indexBuffer )
{
    if( !mCmdList || !indexBuffer ) return;

    mCmdList->IASetIndexBuffer( &indexBuffer->GetView() );
}

// パイプラインステートをセット
void CommandList::SetPipelineState( ComputePSO* pso )
{
    if( !mCmdList || !pso ) return;

    mCmdList->SetPipelineState( pso->GetPipelineState().Get() );
}

// パイプラインステートをセット
void CommandList::SetPipelineState( GraphicsPSO* pso )
{
    if( !mCmdList || !pso ) return;

    mCmdList->SetPipelineState( pso->GetPipelineState().Get() );
}

// プリミティブ型をセット
void CommandList::SetPrimitiveTopology( D3D12_PRIMITIVE_TOPOLOGY primitiveTopology )
{
    if( !mCmdList ) return;

    mCmdList->IASetPrimitiveTopology( primitiveTopology );
}

// レンダーターゲットをセット
void CommandList::SetRenderTarget( uint32_t numRenderTarget, DescriptorHandle* hRTV, DescriptorHandle* hDSV )
{
    if( !mCmdList ) return;

    mCmdList->OMSetRenderTargets(
        numRenderTarget,
        hRTV ? &hRTV->mCPU : nullptr,
        false,
        hDSV ? &hDSV->mCPU : nullptr );
}

// シザー矩形をセット
void CommandList::SetScissorRect( float left, float top, float right, float bottom )
{
    if( !mCmdList ) return;

    D3D12_RECT scissorRc = {};
    scissorRc.left = static_cast<LONG>( left );
    scissorRc.top = static_cast<LONG>( top );
    scissorRc.right = static_cast<LONG>( right );
    scissorRc.bottom = static_cast<LONG>( bottom );
    mCmdList->RSSetScissorRects( 1, &scissorRc );
}

// 頂点バッファをセット
void CommandList::SetVertexBuffer( VertexBuffer* vertexBuffer )
{
    if( !mCmdList || !vertexBuffer ) return;

    mCmdList->IASetVertexBuffers( 0, 1, &vertexBuffer->GetView() );
}

// ビューポートをセット
void CommandList::SetViewport( float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth )
{
    if( !mCmdList ) return;

    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = topLeftX;
    viewport.TopLeftY = topLeftY;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = minDepth;
    viewport.MaxDepth = maxDepth;
    mCmdList->RSSetViewports( 1, &viewport );
}

// Transitionバリア
void CommandList::TransitionBarrier( ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after )
{
    if( !mCmdList ) return;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = resource;
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    mCmdList->ResourceBarrier( 1, &barrier );
}

// UAVバリア
void CommandList::UAVBarrier( ID3D12Resource* resource )
{
    if( !mCmdList ) return;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource = resource;
    mCmdList->ResourceBarrier( 1, &barrier );
}

#pragma endregion

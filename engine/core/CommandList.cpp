#include "CommandList.h"

#include "DirectXBase.h"
#include "GraphicsPSO.h"
#include "RootSignature.h"

// コンストラクタ
CommandList::CommandList()
    : mCmdAllocators()
    , mCmdList( nullptr )
{
}

// 作成
bool CommandList::Create( uint32_t count )
{
    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    mCmdAllocators.resize( count );
    for( auto& cmdAllocator : mCmdAllocators )
    {
        // コマンドアロケーターを作成
        auto hr = device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( cmdAllocator.GetAddressOf() ) );
        if( FAILED( hr ) ) return false;
    }

    // コマンドリストを作成
    auto hr = device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocators[0].Get(), nullptr, IID_PPV_ARGS( mCmdList.GetAddressOf() ) );
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
    if( idx < 0 || idx >= mCmdAllocators.size() ) return;

    if( !mCmdAllocators[idx] || !mCmdList ) return;

    mCmdAllocators[idx]->Reset();
    mCmdList->Reset( mCmdAllocators[idx].Get(), nullptr );
}

#pragma region ID3D12GraphicsCommandListラッパー

// デスクリプタヒープをセット
void CommandList::SetDescriptorHeap( DescriptorHeap* descriptorHeap )
{
    if( !mCmdList || !descriptorHeap ) return;

    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap->GetDescriptorHeap().Get() };
    mCmdList->SetDescriptorHeaps( 1, descriptorHeaps );
}

// リソースバリアをセット
void CommandList::ResourceBarrier( const D3D12_RESOURCE_BARRIER& barrier )
{
    if( !mCmdList ) return;

    mCmdList->ResourceBarrier( 1, &barrier );
}

// レンダーターゲットをセット
void CommandList::SetRenderTarget( DescriptorHandle* hRTV, DescriptorHandle* hDSV )
{
    if( !mCmdList || !hRTV || !hDSV ) return;

    mCmdList->OMSetRenderTargets( 1, &hRTV->mCPU, false, &hDSV->mCPU );
}

// レンダーターゲットをクリア
void CommandList::ClearRenderTargetView( DescriptorHandle* hRTV, const float clearColor[4] )
{
    if( !mCmdList || !hRTV ) return;

    mCmdList->ClearRenderTargetView( hRTV->mCPU, clearColor, 0, nullptr );
}

// 深度バッファをクリア
void CommandList::ClearDepthStencilView( DescriptorHandle* hDSV )
{
    if( !mCmdList || !hDSV ) return;

    mCmdList->ClearDepthStencilView( hDSV->mCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );
}

// デスクリプタテーブルをルートシグネチャへセット
void CommandList::SetGraphicsRootDescriptorTable( uint32_t rootParamIdx, DescriptorHandle* hSRV )
{
    if( !mCmdList || !hSRV ) return;

    mCmdList->SetGraphicsRootDescriptorTable( rootParamIdx, hSRV->mGPU );
}

// 頂点バッファをセット
void CommandList::SetVertexBuffer( const D3D12_VERTEX_BUFFER_VIEW& vbv )
{
    if( !mCmdList ) return;

    mCmdList->IASetVertexBuffers( 0, 1, &vbv );
}

// ルートシグネチャをセット
void CommandList::SetGraphicsRootSignature( RootSignature* rootSignature )
{
    if( !mCmdList || !rootSignature ) return;

    mCmdList->SetGraphicsRootSignature( rootSignature->GetRootSignature().Get() );
}

// パイプラインステートをセット
void CommandList::SetPipelineState( GraphicsPSO* pso )
{
    if( !mCmdList || !pso ) return;

    mCmdList->SetPipelineState( pso->GetPipelineState().Get() );
}

#pragma endregion

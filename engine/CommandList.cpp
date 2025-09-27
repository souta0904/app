#include "CommandList.h"

#include "DescriptorHeap.h"

// コンストラクタ
CommandList::CommandList()
    : mCmdAllocator( nullptr )
    , mCmdList( nullptr )
{
}

// コマンドリストを作成
bool CommandList::Create()
{
    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    // コマンドアロケーターを作成
    auto hr = device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( mCmdAllocator.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // コマンドリストを作成
    hr = device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr, IID_PPV_ARGS( mCmdList.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    return true;
}

// コマンドリストを閉じる
void CommandList::Close()
{
    if( !mCmdList ) return;

    mCmdList->Close();
}

// コマンドリストをリセット
void CommandList::Reset()
{
    if( !mCmdAllocator || !mCmdList ) return;

    mCmdAllocator->Reset();
    mCmdList->Reset( mCmdAllocator.Get(), nullptr );
}

// デスクリプタヒープを設定
void CommandList::SetDescriptorHeap( DescriptorHeap* descriptorHeap )
{
    if( !mCmdList || !descriptorHeap ) return;

    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap->GetDescriptorHeap().Get() };
    mCmdList->SetDescriptorHeaps( 1, descriptorHeaps );
}

#include "CommandQueue.h"

#include "CommandList.h"

// コンストラクタ
CommandQueue::CommandQueue()
    : mCmdQueue( nullptr )
    , mFenceVal( 0 )
    , mFence( nullptr )
    , mFenceEvent( nullptr )
{
}

// デストラクタ
CommandQueue::~CommandQueue()
{
    if( mFenceEvent )
    {
        CloseHandle( mFenceEvent );
        mFenceEvent = nullptr;
    }
}

// コマンドキューを作成
bool CommandQueue::Create()
{
    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    // コマンドキューを作成
    D3D12_COMMAND_QUEUE_DESC desc = {};
    auto hr = device->CreateCommandQueue( &desc, IID_PPV_ARGS( mCmdQueue.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    mFenceVal = 0;

    // フェンスを作成
    hr = device->CreateFence( mFenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( mFence.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // フェンスイベントを作成
    mFenceEvent = CreateEvent( nullptr, false, false, nullptr );
    if( !mFenceEvent ) return false;

    return true;
}

// コマンドリストを実行
void CommandQueue::Execute( CommandList* cmdList )
{
    if( !mCmdQueue || !cmdList ) return;

    ID3D12CommandList* cmdLists[] = { cmdList->GetCmdList().Get() };
    mCmdQueue->ExecuteCommandLists( 1, cmdLists );
}

// GPUの実行完了を待つ
void CommandQueue::WaitForGPU()
{
    if( !mCmdQueue || !mFence || !mFenceEvent ) return;

    ++mFenceVal;
    mCmdQueue->Signal( mFence.Get(), mFenceVal );
    if( mFence->GetCompletedValue() < mFenceVal )
    {
        mFence->SetEventOnCompletion( mFenceVal, mFenceEvent );
        WaitForSingleObject( mFenceEvent, INFINITE );
    }
}

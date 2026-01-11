#include "CommandQueue.h"

#include "CommandList.h"
#include "DirectXBase.h"

// コンストラクタ
CommandQueue::CommandQueue()
    : mCmdQueue( nullptr )
    , mFenceValue( 0 )
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

// 作成
bool CommandQueue::Create( CommandList::Type type )
{
    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    // コマンドキューを作成
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = CommandList::GetCommandListType( type );
    auto hr = device->CreateCommandQueue( &desc, IID_PPV_ARGS( mCmdQueue.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    mFenceValue = 0;

    // フェンスを作成
    hr = device->CreateFence( mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( mFence.GetAddressOf() ) );
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

    mCmdQueue->Signal( mFence.Get(), ++mFenceValue );
}

// 指定したコマンドキューを待つ
void CommandQueue::Wait( CommandQueue* cmdQueue )
{
    if( !cmdQueue ) return;

    mCmdQueue->Wait( cmdQueue->mFence.Get(), cmdQueue->mFenceValue );
}

// GPUの処理を待つ
void CommandQueue::WaitGPU()
{
    if( !mFence || !mFenceEvent ) return;

    if( mFence->GetCompletedValue() < mFenceValue )
    {
        mFence->SetEventOnCompletion( mFenceValue, mFenceEvent );
        WaitForSingleObjectEx( mFenceEvent, INFINITE, false );
    }
}

// GPUの処理を待つ(終了処理用)
void CommandQueue::WaitGPUTerm()
{
    if( !mFence || !mFenceEvent ) return;

    mCmdQueue->Signal( mFence.Get(), ++mFenceValue );
    mFence->SetEventOnCompletion( mFenceValue, mFenceEvent );
    WaitForSingleObjectEx( mFenceEvent, INFINITE, false );
}

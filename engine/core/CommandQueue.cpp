#include "CommandQueue.h"

#include "CommandList.h"
#include "DirectXBase.h"

// コンストラクタ
CommandQueue::CommandQueue()
    : mCmdQueue( nullptr )
    , mFenceValues()
    , mFence( nullptr )
    , mFenceEvent( nullptr )
    , mCurrValue( 0 )
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
bool CommandQueue::Create( uint32_t count )
{
    auto device = DirectXBase::GetInstance().GetDevice();
    if( !device ) return false;

    // コマンドキューを作成
    D3D12_COMMAND_QUEUE_DESC desc = {};
    auto hr = device->CreateCommandQueue( &desc, IID_PPV_ARGS( mCmdQueue.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    mFenceValues.resize( count, 0 );

    // フェンスを作成
    hr = device->CreateFence( mFenceValues[0], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( mFence.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    // フェンスイベントを作成
    mFenceEvent = CreateEvent( nullptr, false, false, nullptr );
    if( !mFenceEvent ) return false;

    return true;
}

// コマンドリストを実行
void CommandQueue::Execute( CommandList* cmdList, uint32_t idx )
{
    if( idx < 0 || idx >= mFenceValues.size() ) return;

    if( !mCmdQueue || !cmdList ) return;

    ID3D12CommandList* cmdLists[] = { cmdList->GetCmdList().Get() };
    mCmdQueue->ExecuteCommandLists( 1, cmdLists );

    mCurrValue = mFenceValues[idx];
    mCmdQueue->Signal( mFence.Get(), mCurrValue );
}

// GPUの処理を待つ
void CommandQueue::WaitGPU( uint32_t idx )
{
    if( idx < 0 || idx >= mFenceValues.size() ) return;

    if( !mFence || !mFenceEvent ) return;

    if( mFence->GetCompletedValue() < mFenceValues[idx] )
    {
        mFence->SetEventOnCompletion( mFenceValues[idx], mFenceEvent );
        WaitForSingleObjectEx( mFenceEvent, INFINITE, false );
    }

    mFenceValues[idx] = mCurrValue + 1;
}

// GPUの処理を待つ(終了処理用)
void CommandQueue::WaitGPUTerm( uint32_t idx )
{
    if( idx < 0 || idx >= mFenceValues.size() ) return;

    if( !mFence || !mFenceEvent ) return;

    mCmdQueue->Signal( mFence.Get(), mFenceValues[idx] );
    mFence->SetEventOnCompletion( mFenceValues[idx], mFenceEvent );
    WaitForSingleObjectEx( mFenceEvent, INFINITE, false );
    ++mFenceValues[idx];
}

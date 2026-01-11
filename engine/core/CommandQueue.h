#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <vector>

#include "CommandList.h"

/// <summary>
/// コマンドキュー
/// </summary>
class CommandQueue
{
   private:
    // コマンドキュー
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue;
    // フェンス値
    uint64_t mFenceValue;
    // フェンス
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    // フェンスイベント
    HANDLE mFenceEvent;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    CommandQueue();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CommandQueue();

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="type">コマンドリスト種別</param>
    /// <returns>成否</returns>
    bool Create( CommandList::Type type );

    /// <summary>
    /// コマンドリストを実行
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Execute( CommandList* cmdList );

    /// <summary>
    /// 指定したコマンドキューを待つ
    /// </summary>
    /// <param name="cmdQueue">対象のコマンドキュー</param>
    void Wait( CommandQueue* cmdQueue );

    /// <summary>
    /// GPUの処理を待つ
    /// </summary>
    void WaitGPU();

    /// <summary>
    /// GPUの処理を待つ(終了処理用)
    /// </summary>
    void WaitGPUTerm();

    /// <summary>コマンドキューを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCmdQueue() const { return mCmdQueue; }
};

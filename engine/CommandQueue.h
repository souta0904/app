#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

class CommandList;

/// <summary>
/// コマンドキュー
/// </summary>
class CommandQueue
{
   private:
    // コマンドキュー
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue;
    // フェンス値
    uint64_t mFenceVal;
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
    /// コマンドキューを作成
    /// </summary>
    /// <returns>成否</returns>
    bool Create();

    /// <summary>
    /// コマンドリストを実行
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Execute( CommandList* cmdList );

    /// <summary>
    /// GPUの実行完了を待つ
    /// </summary>
    void WaitForGPU();
};

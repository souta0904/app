#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <vector>

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
    std::vector<uint64_t> mFenceValues;
    // フェンス
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    // フェンスイベント
    HANDLE mFenceEvent;

    // 現在のフェンス値
    uint64_t mCurrValue;

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
    /// <param name="count">バックバッファの数</param>
    /// <returns>成否</returns>
    bool Create( uint32_t count );

    /// <summary>
    /// コマンドリストを実行
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="idx">バックバッファのインデックス</param>
    void Execute( CommandList* cmdList, uint32_t idx );

    /// <summary>
    /// GPUの処理を待つ
    /// </summary>
    /// <param name="idx">バックバッファのインデックス</param>
    void WaitGPU( uint32_t idx );

    /// <summary>
    /// GPUの処理を待つ(終了処理用)
    /// </summary>
    /// <param name="idx">バックバッファのインデックス</param>
    void WaitGPUTerm( uint32_t idx );

    /// <summary>コマンドキューを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCmdQueue() const { return mCmdQueue; }
};

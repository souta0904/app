#pragma once
#include <d3d12.h>
#include <wrl.h>

class DescriptorHeap;

/// <summary>
/// コマンドリスト
/// </summary>
class CommandList
{
   private:
    // コマンドアロケーター
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCmdAllocator;
    // コマンドリスト
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    CommandList();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CommandList() = default;

    /// <summary>
    /// コマンドリストを作成
    /// </summary>
    /// <returns>成否</returns>
    bool Create();

    /// <summary>
    /// コマンドリストを閉じる
    /// </summary>
    void Close();

    /// <summary>
    /// コマンドリストをリセット
    /// </summary>
    void Reset();

    /// <summary>
    /// デスクリプタヒープを設定
    /// </summary>
    /// <param name="descriptorHeap">デスクリプタヒープ</param>
    void SetDescriptorHeap( DescriptorHeap* descriptorHeap );
};

#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <vector>

#include "DescriptorHeap.h"

/// <summary>
/// コマンドリスト
/// </summary>
class CommandList
{
   private:
    // コマンドアロケーター
    std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCmdAllocators;
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
    /// 作成
    /// </summary>
    /// <param name="count">コマンドアロケーターの数</param>
    /// <returns>成否</returns>
    bool Create( uint32_t count = 1 );

    /// <summary>
    /// クローズ
    /// </summary>
    void Close();

    /// <summary>
    /// リセット
    /// </summary>
    /// <param name="idx">インデックス</param>
    void Reset( uint32_t idx = 0 );

#pragma region ID3D12GraphicsCommandListラッパー

    /// <summary>
    /// デスクリプタヒープをセット
    /// </summary>
    /// <param name="descriptorHeap">デスクリプタヒープ</param>
    void SetDescriptorHeap( DescriptorHeap* descriptorHeap );

    /// <summary>
    /// リソースバリアをセット
    /// </summary>
    /// <param name="barrier">リソースバリア</param>
    void ResourceBarrier( const D3D12_RESOURCE_BARRIER& barrier );

    /// <summary>
    /// レンダーターゲットをセット
    /// </summary>
    /// <param name="hRTV">RTVのCPUデスクリプタハンドル</param>
    /// <param name="hDSV">DSVのCPUデスクリプタハンドル</param>
    void SetRenderTarget( DescriptorHandle* hRTV, DescriptorHandle* hDSV );

    /// <summary>
    /// レンダーターゲットをクリア
    /// </summary>
    /// <param name="hRTV">RTVのCPUデスクリプタハンドル</param>
    /// <param name="clearColor">クリアカラー</param>
    void ClearRenderTargetView( DescriptorHandle* hRTV, const float clearColor[4] );

    /// <summary>
    /// 深度バッファをクリア
    /// </summary>
    /// <param name="hDSV">DSVのCPUデスクリプタハンドル</param>
    void ClearDepthStencilView( DescriptorHandle* hDSV );

#pragma endregion

    /// <summary>コマンドリストを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCmdList() const { return mCmdList; }
};

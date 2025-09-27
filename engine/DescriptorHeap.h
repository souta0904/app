#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <memory>
#include <vector>

class CommandList;

/// <summary>
/// デスクリプタハンドル
/// </summary>
struct DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE mCPU = {};
    D3D12_GPU_DESCRIPTOR_HANDLE mGPU = {};
    bool mIsActive = false;
    uint32_t mIndex = 0;
};

/// <summary>
/// デスクリプタヒープ
/// </summary>
class DescriptorHeap
{
   public:
    /// <summary>
    /// デスクリプタヒープの種類
    /// </summary>
    enum class Type
    {
        kCBV_SRV_UAV,
        kSAMPLER,
        kRTV,
        kDSV,
    };

   private:
    // デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
    // デスクリプタハンドルの配列
    std::vector<DescriptorHandle> mDescriptorHdls;
    // 次に割り当てるインデックス
    uint32_t mNextIndex;
    // デスクリプタのインクリメントサイズ
    uint32_t mIncrementSize;
    // シェーダーから参照可能か
    bool mIsShaderVisible;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    DescriptorHeap();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~DescriptorHeap() = default;

    /// <summary>
    /// デスクリプタヒープを作成
    /// </summary>
    /// <param name="type">デスクリプタヒープの種類</param>
    /// <param name="numDescriptors">デスクリプタの数</param>
    /// <param name="isShaderVisible">シェーダーから参照可能か</param>
    /// <returns>成否</returns>
    bool Create( Type type, uint32_t numDescriptors, bool isShaderVisible );

    /// <summary>
    /// デスクリプタヒープをバインド
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Bind( CommandList* cmdList );

    /// <summary>
    /// デスクリプタハンドルを割り当て
    /// </summary>
    /// <returns>割り当てたデスクリプタハンドル</returns>
    DescriptorHandle* Alloc();

    /// <summary>
    /// デスクリプタハンドルを解放
    /// </summary>
    /// <param name="hdl">解放するデスクリプタハンドル</param>
    void Free( DescriptorHandle*& hdl );

    /// <summary>デスクリプタヒープを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return mDescriptorHeap; }

   private:
    /// <summary>
    /// デスクリプタハンドルを初期化
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <param name="initHdl">初期化するデスクリプタハンドル</param>
    void InitHdl( uint32_t index, DescriptorHandle& initHdl );
};

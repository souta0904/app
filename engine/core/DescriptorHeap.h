#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <memory>
#include <vector>

#include "utils/SimplePool.h"

class CommandList;

/// <summary>
/// デスクリプタハンドル
/// </summary>
struct DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE mCPU = {};
    D3D12_GPU_DESCRIPTOR_HANDLE mGPU = {};
    uint32_t mIdx = 0;
};

/// <summary>
/// デスクリプタヒープ
/// </summary>
class DescriptorHeap
{
   public:
    /// <summary>
    /// 種別
    /// </summary>
    enum class Type
    {
        CBV_SRV_UAV,
        SAMPLER,
        RTV,
        DSV,
    };

   private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
    std::unique_ptr<Nebula::SimplePool<DescriptorHandle>> mDescriptorHdlPool;
    uint32_t mIncrementSize;
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
    /// 作成
    /// </summary>
    /// <param name="type">デスクリプタヒープの種類</param>
    /// <param name="numDescriptors">デスクリプタの数</param>
    /// <param name="isShaderVisible">シェーダーから参照可能か</param>
    /// <returns>成否</returns>
    bool Create( Type type, uint32_t numDescriptors, bool isShaderVisible );

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
    /// <param name="idx">インデックス</param>
    /// <param name="initHdl">初期化するデスクリプタハンドル</param>
    void InitHdl( uint32_t idx, DescriptorHandle& initHdl );
};

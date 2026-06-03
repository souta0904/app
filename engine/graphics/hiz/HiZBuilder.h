#pragma once
#include <d3d12.h>

#include <memory>

#include "core/ComputePSO.h"
#include "core/ConstantBuffer.h"
#include "core/DescriptorHeap.h"
#include "core/RootSignature.h"

/// <summary>
/// Hierarchical-Zの構築
/// </summary>
class HiZBuilder
{
    friend class OcclusionCulling;

   private:
    /// <summary>
    /// ミップサイズ
    /// </summary>
    struct MipSize
    {
        uint32_t mSizeX;
        uint32_t mSizeY;

        float pad[2];
    };

    // PSO
    std::unique_ptr<RootSignature> mRS;
    std::unique_ptr<ComputePSO> mPSO;

    // ミップレベル
    uint32_t mMipLevel;

    Microsoft::WRL::ComPtr<ID3D12Resource> mHiZBuff;
    std::vector<DescriptorHandle> mSRVs;
    std::vector<DescriptorHandle> mUAVs;
    std::vector<std::unique_ptr<ConstantBuffer>> mCBs;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    HiZBuilder();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~HiZBuilder() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// Hierarchical-Zを構築
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Build( CommandList* cmdList );

    uint32_t GetMipLevel();

    DescriptorHandle GetSRV() const { return mSRVs[0]; }

   private:
    /// <summary>
    /// PSOの作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreatePSO();

    /// <summary>
    /// バッファとビューの作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateBuff();

    /// <summary>
    /// 深度をコピー
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void CopyDepth( CommandList* cmdList );
};

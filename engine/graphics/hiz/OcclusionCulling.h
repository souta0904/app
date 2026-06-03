#pragma once
#include "HiZBuilder.h"
#include "core/ComputePSO.h"
#include "core/ConstantBuffer.h"
#include "core/DescriptorHeap.h"
#include "core/RWStructuredBuffer.h"
#include "core/ReadbackBuffer.h"
#include "core/RootSignature.h"
#include "core/StructuredBuffer.h"
#include "math/Matrix4.h"
#include "math/Primitive.h"
#include "math/Vector2.h"

class Camera;
class CommandList;
class HiZBuilder;

/// <summary>
/// オクルージョンカリング
/// </summary>
class OcclusionCulling
{
   private:
    static const uint32_t kMaxObjCount = 2048;

    /// <summary>
    /// 定数
    /// </summary>
    struct Constant
    {
        Matrix4 mVP;
        Vector2 mScreenSize;
        uint32_t mMipLevel;

        float pad;
    };

    // PSO
    std::unique_ptr<RootSignature> mRS;
    std::unique_ptr<ComputePSO> mPSO;

    std::unique_ptr<HiZBuilder> mHiZBuilder;
    std::unique_ptr<StructuredBuffer> mBoxBuff;
    std::unique_ptr<RWStructuredBuffer> mResultBuff;
    std::unique_ptr<ConstantBuffer> mCB;
    DescriptorHandle mBoxSRV;
    DescriptorHandle mResultUAV;

    std::unique_ptr<ReadbackBuffer> mResultReadback;
    std::vector<bool> mResult;
    uint32_t mVisibleCount;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    OcclusionCulling();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~OcclusionCulling() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// 実行
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="boxes">バウンディングボックス</param>
    /// <param name="camera">カメラ</param>
    void Execute( CommandList* cmdList, const std::vector<AABB3D>& boxes, Camera* camera );

    /// <summary>結果を取得</summary>
    const std::vector<bool>& GetResult() const { return mResult; }

    uint32_t GetVisibleCount() const { return mVisibleCount; }

   private:
    /// <summary>
    /// PSOの作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreatePSO();
};

#pragma once
#include <d3d12.h>

#include <array>
#include <memory>
#include <unordered_map>

#include "core/ComputePSO.h"
#include "core/DescriptorHeap.h"
#include "core/GraphicsPSO.h"
#include "core/RWStructuredBuffer.h"
#include "core/RootSignature.h"
#include "core/StructuredBuffer.h"
#include "graphics/model/ModelData.h"

class Camera;
class HiZBuilder;

/// <summary>
/// 草の描画
/// </summary>
class GrassRenderer
{
   private:
    static const uint32_t kMax = 100 * 100;

    struct IndirectCommand
    {
        uint32_t mInstanceIndex;
        D3D12_DRAW_INDEXED_ARGUMENTS mDrawArg;
    };

    struct TransformationMatrix
    {
        Matrix4 mWorld;
    };
    TransformationMatrix mat[kMax];

    struct Scene
    {
        Matrix4 mVP;
        float mTime;
    };

    struct InstanceData
    {
        Vector3 mMin;
        Vector3 mMax;
    };

    struct Frustum
    {
        Vector4 mPlanes[6];

        Matrix4 mVP;
        Vector2 mScreenSize;
        uint32_t mMipLevel;

        float pad;
    };

    std::unique_ptr<RootSignature> mIndirectRS;
    std::unique_ptr<ComputePSO> mIndirectPSO;
    std::unique_ptr<RootSignature> mRS;
    std::unique_ptr<GraphicsPSO> mPSO;
    Microsoft::WRL::ComPtr<ID3D12CommandSignature> mCommandSignature;

    std::unique_ptr<RWStructuredBuffer> mIndirectBuff;
    Microsoft::WRL::ComPtr<ID3D12Resource> mCounterResetBuff;
    std::unique_ptr<StructuredBuffer> mTransformationBuff;
    std::unique_ptr<ConstantBuffer> mSceneBuff;
    std::unique_ptr<ConstantBuffer> mCameraBuff;
    std::unique_ptr<StructuredBuffer> mInstanceDataBuff;
    Scene mScene;
    DescriptorHandle* mInstanceSRVHdl;
    DescriptorHandle* mSRVHdl;
    DescriptorHandle* mUAVHdl;

    ModelData* mModel;
    float mRotate[kMax];

    // コマンドリスト
    CommandList* mCmdList;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static GrassRenderer& GetInstance()
    {
        static GrassRenderer instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    GrassRenderer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GrassRenderer() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    GrassRenderer( const GrassRenderer& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    GrassRenderer& operator=( const GrassRenderer& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    GrassRenderer( GrassRenderer&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    GrassRenderer& operator=( GrassRenderer&& ) = delete;

   public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    void Update(float deltaTime);

    /// <summary>
    /// 描画開始
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Begin( CommandList* cmdList );

    /// <summary>
    /// 描画終了
    /// </summary>
    void End();

    void Draw( Camera* camera, Camera* fCamera, HiZBuilder* hiz );

    AABB3D GetWorldAABB(const AABB3D& local, const Matrix4& world);

    Microsoft::WRL::ComPtr<ID3D12CommandSignature> GetCommandSignature() const { return mCommandSignature; }
};

#pragma once
#include <memory>
#include <vector>

#include "core/ConstantBuffer.h"
#include "core/RWStructuredBuffer.h"
#include "core/StructuredBuffer.h"
#include "math/Primitive.h"
#include "ModelInstance.h"

class Camera;
class CommandList;
class Material;
class Mesh;

/// <summary>
/// メッシュのソーター
/// </summary>
class MeshSorter
{
   public:
    /// <summary>
    /// コマンド構造体
    /// </summary>
    struct IndirectCommand
    {
        D3D12_VERTEX_BUFFER_VIEW mVBV;
        D3D12_INDEX_BUFFER_VIEW mIBV;
        D3D12_GPU_VIRTUAL_ADDRESS mTransMatCBV;
        uint32_t mMaterialIdx;
        D3D12_DRAW_INDEXED_ARGUMENTS mDrawArg;
    };

   private:
    static const uint32_t kMaxDrawCount = 4096;

    /// <summary>
    /// 描画アイテム
    /// </summary>
    struct SortItem
    {
        uint64_t mPSOKey;
        float mDistance;
        uint32_t mQuantizedDist;
        ConstantBuffer* mTransMatCB;
        Mesh* mMesh;
        Material* mMaterial;
        AABB3D mWorldAABB;
        bool mIsVisible = true;
        ModelInstance* inst;
    };

    // カメラ
    Camera* mCamera;
    Camera* mFrustumCamera;
    // カメラ用定数バッファ
    std::unique_ptr<ConstantBuffer> mCameraCB;

    // 描画アイテムリスト
    std::vector<SortItem> mSortItems;

    // インダイレクト描画
    bool mUseIndirectDraw = true;
    std::unique_ptr<RWStructuredBuffer> mIndirectBuff;
    std::unique_ptr<StructuredBuffer> mIndirectUpload;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    MeshSorter();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~MeshSorter() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="camera">カメラ</param>
    /// <returns>成否</returns>
    bool Init( Camera* camera );

    /// <summary>
    /// 描画アイテムの追加
    /// </summary>
    /// <param name="psoKey">PSOキー</param>
    /// <param name="distance">カメラからの距離</param>
    /// <param name="transMatCB">変換行列用定数バッファ</param>
    /// <param name="mesh">メッシュ</param>
    /// <param name="material">マテリアル</param>
    void Add( uint64_t psoKey, float distance, ConstantBuffer* transMatCB, Mesh* mesh, Material* material, const AABB3D& aabb ,ModelInstance* inst);

    /// <summary>
    /// ソート
    /// </summary>
    void Sort();

    /// <summary>
    /// z-prepass描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void RenderZPrepass( CommandList* cmdList );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Render( CommandList* cmdList );

    /// <summary>カメラを取得</summary>
    Camera* GetCamera() const { return mCamera; }

    Camera* GetFrustumCamera() const { return mFrustumCamera; }

    uint32_t GetItemCount() const { return static_cast<uint32_t>( mSortItems.size() ); }

    /// <summary>カメラを設定</summary>
    void SetCamera( Camera* camera ) { mCamera = camera; }

    void SetFrustumCamera( Camera* camera ) { mFrustumCamera = camera; }

   private:
    /// <summary>
    /// 距離を量子化
    /// </summary>
    /// <param name="distance">距離</param>
    /// <returns>量子化された距離</returns>
    uint32_t QuantizeDist( float distance );

    void UpdateIndirectBuff( CommandList* cmdList );
};

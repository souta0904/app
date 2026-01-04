#pragma once
#include <memory>
#include <vector>

#include "RenderQueue.h"
#include "core/ConstantBuffer.h"

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
    };

    // カメラ
    Camera* mCamera;
    // カメラ用定数バッファ
    std::unique_ptr<ConstantBuffer> mCameraCB;

    // 描画アイテムリスト
    std::vector<SortItem> mSortItems;

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
    bool Initialize( Camera* camera );

    /// <summary>
    /// 描画アイテムの追加
    /// </summary>
    /// <param name="psoKey">PSOキー</param>
    /// <param name="distance">カメラからの距離</param>
    /// <param name="transMatCB">変換行列用定数バッファ</param>
    /// <param name="mesh">メッシュ</param>
    /// <param name="material">マテリアル</param>
    void Add( uint64_t psoKey, float distance, ConstantBuffer* transMatCB, Mesh* mesh, Material* material );

    /// <summary>
    /// ソート
    /// </summary>
    void Sort();

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Render( CommandList* cmdList );

    /// <summary>カメラを取得</summary>
    Camera* GetCamera() const { return mCamera; }

    /// <summary>カメラを設定</summary>
    void SetCamera( Camera* camera ) { mCamera = camera; }

   private:
    /// <summary>
    /// 距離を量子化
    /// </summary>
    /// <param name="distance">距離</param>
    /// <returns>量子化された距離</returns>
    uint32_t QuantizeDist( float distance );
};

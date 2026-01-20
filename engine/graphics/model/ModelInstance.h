#pragma once
#include "ModelData.h"

class Camera;
class MeshSorter;

/// <summary>
/// モデルインスタンス
/// </summary>
class ModelInstance
{
   private:
    /// <summary>
    /// 変換行列
    /// </summary>
    struct TransformationMatrix
    {
        Matrix4 mWorld;
        Matrix4 mWVP;
        Matrix4 mWorldInvTranspose;
    };

    // モデルデータ
    ModelData* mModelData;
    // ノードリスト
    std::vector<ModelNode> mNodes;
    // 変換行列
    std::vector<std::unique_ptr<ConstantBuffer>> mTransMatCBs;
    // マテリアルリスト
    std::vector<Material*> mMaterials;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ModelInstance();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ModelInstance() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="modelData">モデルデータ</param>
    /// <returns>成否</returns>
    bool Create( ModelData* modelData );

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update( float deltaTime );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="sorter">ソーター</param>
    /// <param name="worldMat">ワールド行列</param>
    void Draw( MeshSorter* sorter, const Matrix4& worldMat );

    /// <summary>
    /// マテリアルを取得
    /// </summary>
    /// <param name="idx">インデックス</param>
    /// <returns>マテリアル</returns>
    Material* GetMaterial( uint32_t idx );

    /// <summary>
    /// マテリアルを設定
    /// </summary>
    /// <param name="idx">インデックス</param>
    /// <param name="material">マテリアル</param>
    void SetMaterial( uint32_t idx, Material* material );

    /// <summary>マテリアル数を取得</summary>
    uint32_t GetMaterialCount() const { return static_cast<uint32_t>( mMaterials.size() ); }

   private:
    /*
    /// <summary>
    /// 定数バッファを更新
    /// </summary>
    /// <param name="worldMat">ワールド行列</param>
    /// <param name="camera">カメラ</param>
    void UpdateCB( const Matrix4& worldMat, Camera* camera );
    */
};

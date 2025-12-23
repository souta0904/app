#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Material.h"
#include "Mesh.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"

/// <summary>
/// モデルノード
/// </summary>
struct ModelNode
{
    // ノード名
    std::string mName;
    // トランスフォーム
    // スケール
    Vector3 mScale;
    // 回転
    Quaternion mRotate;
    // 座標
    Vector3 mTranslate;
    // ローカル行列
    Matrix4 mLocalMat;
    // モデル行列
    Matrix4 mModelMat;
    // 親子構造
    // 自分のインデックス
    int32_t mIndex;
    // 親のインデックス
    std::optional<int32_t> mParent;
    // 子のインデックス
    std::vector<int32_t> mChildren;
};

/// <summary>
/// モデルデータ
/// </summary>
class ModelData
{
    friend class ModelInstance;

   private:
    /// <summary>
    /// メッシュデータ
    /// </summary>
    struct MeshData
    {
        // ノードのインデックス
        int32_t mNodeIdx;
        // メッシュ
        std::unique_ptr<Mesh> mMesh;
    };

    // ビルド済みか
    bool mIsBuilt;
    // assimpシーン
    const aiScene* mAssimpScene;
    // ヘルパー変数
    // ノードインデックス to assimpノード
    std::unordered_map<int32_t, aiNode*> mNodeIdxToAssimpNode;
    // ノード名 to ノードインデックス
    std::unordered_map<std::string, int32_t> mNodeNameToNodeIdx;

    // モデルパス
    std::string mPath;
    // ノードリスト
    std::vector<ModelNode> mNodes;
    // ルートノードのインデックス
    int32_t mRootNodeIdx;
    // メッシュ数
    uint32_t mMeshCount;
    // メッシュデータ
    std::vector<MeshData> mMeshes;
    // マテリアル数
    uint32_t mMaterialCount;
    // マテリアルリスト
    std::vector<std::unique_ptr<Material>> mMaterials;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ModelData();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ModelData() = default;

    /// <summary>
    /// モデルを構築
    /// </summary>
    /// <param name="path">モデルのパス</param>
    /// <returns>成否</returns>
    bool Build( const std::string& path );

   private:
    /// <summary>
    /// ノードを構築(再帰)
    /// </summary>
    /// <param name="assimpNode">assimpノード</param>
    /// <param name="parent">親のインデックス</param>
    /// <returns>自分のインデックス</returns>
    int32_t BuildNode( aiNode* assimpNode, const std::optional<int32_t>& parent );

    /// <summary>
    /// メッシュを構築(再帰)
    /// </summary>
    /// <param name="node">ノード</param>
    void BuildMesh( ModelNode* node );

    /// <summary>
    /// マテリアルを構築
    /// </summary>
    void BuildMaterial();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

#ifdef _DEBUG
    /// <summary>
    /// ノードの階層をデバッグ表示
    /// </summary>
    /// <param name="node">ノード</param>
    /// <param name="depth">インデント用深度</param>
    void OutputNodeInfo( ModelNode* node, uint32_t depth = 0 );
#endif
};

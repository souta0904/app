#pragma once
#include <memory>
#include <string>
#include <vector>

#include "PSOKey.h"
#include "core/IndexBuffer.h"
#include "core/VertexBuffer.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

class CommandList;

/// <summary>
/// メッシュ
/// </summary>
class Mesh
{
    friend class ModelData;
    friend class ModelInstance;

   public:
    /// <summary>
    /// 頂点データ
    /// </summary>
    struct Vertex
    {
        Vector4 mPosition;
        Vector3 mNormal;
        Vector2 mUV;
    };

   private:
    // メッシュ名
    std::string mName;

    // メッシュフラグ
    MeshFlags mFlags;
    // 頂点データ
    std::vector<Vertex> mVertices;
    // 頂点バッファ
    std::unique_ptr<VertexBuffer> mVB;
    // 頂点インデックスデータ
    std::vector<uint32_t> mIndices;
    // インデックスバッファ
    std::unique_ptr<IndexBuffer> mIB;

    // マテリアルのインデックス
    uint32_t mMaterialIdx;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Mesh();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Mesh() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="flags">メッシュフラグ</param>
    /// <param name="vertices">頂点データ</param>
    /// <returns>成否</returns>
    bool Create( MeshFlags flags, const std::vector<Vertex>& vertices );

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="flags">メッシュフラグ</param>
    /// <param name="vertices">頂点データ</param>
    /// <param name="indices">頂点インデックスデータ</param>
    /// <returns>成否</returns>
    bool Create( MeshFlags flags, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Draw( CommandList* cmdList );

   private:
    /// <summary>
    /// 頂点バッファを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateVB();

    /// <summary>
    /// インデックスバッファを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateIB();
};

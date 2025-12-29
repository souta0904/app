#pragma once
#include <memory>
#include <string>

#include "PSOKey.h"
#include "RenderQueue.h"
#include "core/ConstantBuffer.h"
#include "math/Color.h"

class CommandList;
class Texture;

/// <summary>
/// 鏡面反射種別
/// </summary>
enum class SpecularType
{
    None,
    Phong,
    BlinnPhong,
};

/// <summary>
/// マテリアル
/// </summary>
class Material
{
    friend class ModelData;
    friend class ModelInstance;

   private:
    /// <summary>
    /// 定数データ
    /// </summary>
    struct Constant
    {
        // 色
        Color mColor;
        // ライティング有無
        uint32_t mIsLighting = true;
        // 鏡面反射種別
        SpecularType mSpecularType = SpecularType::BlinnPhong;
        // 光沢度
        float mShininess = 192.0f;
    };

   private:
    // マテリアル名
    std::string mName;
    // レンダーキュー
    RenderQueue mRenderQueue;
    // マテリアルフラグ
    MaterialFlags mFlags;
    // 定数バッファ
    std::unique_ptr<ConstantBuffer> mCB;
    // テクスチャ
    Texture* mTexture;

   public:
    // 色
    Color mColor;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Material();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Material() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="texture">テクスチャ</param>
    /// <returns>成否</returns>
    bool Create( Texture* texture );

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// バインド
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="constIdx">定数のルートパラメータインデックス</param>
    void Bind( CommandList* cmdList, uint32_t constIdx );

    /// <summary>
    /// バインド
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="constIdx">定数のルートパラメータインデックス</param>
    /// <param name="texIdx">テクスチャのルートパラメータインデックス</param>
    void Bind( CommandList* cmdList, uint32_t constIdx, uint32_t texIdx );

    /// <summary>
    /// NoCullingを設定
    /// </summary>
    /// <param name="enable">有効にするか</param>
    void EnableNoCulling( bool enable );

    /// <summary>
    /// Wireframeを設定
    /// </summary>
    /// <param name="enable">有効にするか</param>
    void EnableWireframe( bool enable );

    /// <summary>レンダーキューを取得</summary>
    RenderQueue GetRenderQueue() const { return mRenderQueue; }

    /// <summary>レンダーキューを設定</summary>
    void SetRenderQueue( RenderQueue renderQueue ) { mRenderQueue = renderQueue; }

    /// <summary>テクスチャを設定</summary>
    void SetTexture( Texture* texture );

   private:
    /// <summary>フラグが立っているか</summary>
    bool HasFlags( MaterialFlags flags ) { return ( mFlags & flags ) == flags; }

    /// <summary>フラグを追加</summary>
    void AddFlags( MaterialFlags flags ) { mFlags |= flags; }

    /// <summary>フラグを削除</summary>
    void RemoveFlags( MaterialFlags flags ) { mFlags &= ~flags; }
};

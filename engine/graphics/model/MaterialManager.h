#pragma once
#include <vector>

#include "Material.h"
#include "core/DescriptorHeap.h"
#include "core/StructuredBuffer.h"

/// <summary>
/// マテリアル管理
/// </summary>
class MaterialManager
{
   private:
    static const uint32_t kMaxMaterialCount = 4096;

    /// <summary>
    /// マテリアルデータ
    /// </summary>
    struct MaterialData
    {
        Color mColor;
        uint32_t mIsLighting;
        SpecularType mSpecularType;
        float mShininess;
        uint32_t mTextureIdx;

        //float pad[3];
    };

    // マテリアル
    std::vector<Material*> mMaterials;
    // マテリアルバッファ
    std::unique_ptr<StructuredBuffer> mMaterialBuff;
    // デスクリプタハンドル
    DescriptorHandle* mSRVHdl;

   public:
    /// <summary>
    /// インスタンスの取得
    /// </summary>
    static MaterialManager& GetInstance()
    {
        static MaterialManager instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    MaterialManager();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~MaterialManager() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    MaterialManager( const MaterialManager& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    MaterialManager& operator=( const MaterialManager& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    MaterialManager( MaterialManager&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    MaterialManager& operator=( MaterialManager&& ) = delete;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// 登録
    /// </summary>
    void Register( Material* material );

    /// <summary>
    /// 解除
    /// </summary>
    void Unregister( Material* material );

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// バインド
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="srvIdx">ルートパラメータインデックス</param>
    void Bind( CommandList* cmdList, uint32_t srvIdx );

   private:
    /// <summary>
    /// インデックスを再構築
    /// </summary>
    void RebuildIdx();
};

#pragma once
#include <memory>
#include <vector>

#include "DirectionalLight.h"
#include "core/ConstantBuffer.h"
#include "math/Color.h"
#include "math/Vector3.h"

class CommandList;

/// <summary>
/// ライト管理
/// </summary>
class LightManager
{
   private:
    // 平行光源の最大数
    static const uint32_t kMaxDirectionalLightCount = 8;

    /// <summary>
    /// 平行光源用
    /// </summary>
    struct DirectionalLightConstant
    {
        // 色
        Color mColor;
        // 向き
        Vector3 mDirection;
        // 明るさ
        float mIntensity;
    };

    /// <summary>
    /// 定数
    /// </summary>
    struct Constant
    {
        // 平行光源のリスト
        DirectionalLightConstant mDirectionalLights[kMaxDirectionalLightCount];
        // 平行光源の数
        uint32_t mDirectionalLightCount;
    };

    // 定数バッファ
    std::unique_ptr<ConstantBuffer> mCB;
    // 平行光源
    std::vector<DirectionalLight*> mDirectionalLights;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static LightManager& GetInstance()
    {
        static LightManager instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    LightManager();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~LightManager() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    LightManager( const LightManager& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    LightManager& operator=( const LightManager& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    LightManager( LightManager&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    LightManager& operator=( LightManager&& ) = delete;

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

    /// <summary>
    /// バインド
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="constIdx">定数のルートパラメータインデックス</param>
    void Bind( CommandList* cmdList, uint32_t constIdx );

    /// <summary>
    /// 平行光源を追加
    /// </summary>
    /// <param name="directionalLight">平行光源</param>
    void AddDirectionalLight( DirectionalLight* directionalLight );

    /// <summary>
    /// 平行光源を削除
    /// </summary>
    /// <param name="directionalLight">平行光源</param>
    void RemoveDirectionalLight( DirectionalLight* directionalLight );

   private:
    /// <summary>
    /// 定数バッファを更新
    /// </summary>
    void UpdateCB();
};

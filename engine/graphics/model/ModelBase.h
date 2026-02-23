#pragma once
#include <d3d12.h>

#include <array>
#include <memory>
#include <unordered_map>

#include "PSOKey.h"
#include "core/GraphicsPSO.h"
#include "core/RootSignature.h"

/// <summary>
/// モデル基盤
/// </summary>
class ModelBase
{
   private:
    // ルートシグネチャ
    std::unique_ptr<RootSignature> mRS;
    // パイプラインステート
    std::unordered_map<uint64_t, std::unique_ptr<GraphicsPSO>> mPSO;

    // z-prepass
    std::unique_ptr<RootSignature> mZPrepassRS;
    std::unique_ptr<GraphicsPSO> mZPrepassPSO;

    // コマンドリスト
    CommandList* mCmdList;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static ModelBase& GetInstance()
    {
        static ModelBase instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ModelBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ModelBase() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    ModelBase( const ModelBase& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    ModelBase& operator=( const ModelBase& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    ModelBase( ModelBase&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    ModelBase& operator=( ModelBase&& ) = delete;

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
    /// 描画開始
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Begin( CommandList* cmdList );

    /// <summary>
    /// 描画終了
    /// </summary>
    void End();

    /// <summary>
    /// z-prepass開始
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void BeginZPrepass( CommandList* cmdList );

    /// <summary>
    /// z-prepass終了
    /// </summary>
    void EndZPrepass();

    /// <summary>
    /// パイプラインステートの設定
    /// </summary>
    /// <param name="psoKey">PSOキー</param>
    void SetGraphicsPSO( uint64_t psoKey );

   private:
    /// <summary>
    /// パイプラインステートの作成
    /// </summary>
    /// <param name="psoKey">PSOキー</param>
    void CreateGraphicsPSO( uint64_t psoKey );

    /// <summary>
    /// z-prepassの初期化
    /// </summary>
    /// <returns>成否</returns>
    bool InitZPrepass();
};

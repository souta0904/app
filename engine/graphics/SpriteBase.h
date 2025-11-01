#pragma once
#include <d3d12.h>

#include <array>
#include <memory>

#include "Sprite.h"
#include "core/GraphicsPSO.h"
#include "core/RootSignature.h"

/// <summary>
/// スプライト基盤
/// </summary>
class SpriteBase
{
    friend class Sprite;

   private:
    // ルートシグネチャ
    std::unique_ptr<RootSignature> mRS;
    // パイプラインステート
    std::array<std::unique_ptr<GraphicsPSO>, static_cast<size_t>( Sprite::BlendMode::Max )> mPSO;

    // コマンドリスト
    CommandList* mCmdList;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static SpriteBase& GetInstance()
    {
        static SpriteBase instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SpriteBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SpriteBase() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    SpriteBase( const SpriteBase& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    SpriteBase& operator=( const SpriteBase& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    SpriteBase( SpriteBase&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    SpriteBase& operator=( SpriteBase&& ) = delete;

   public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="vs">頂点シェーダー</param>
    /// <param name="ps">ピクセルシェーダー</param>
    /// <returns>成否</returns>
    bool Init( ShaderObject* vs, ShaderObject* ps );

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// 描画開始
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="defaultBlend">デフォルトのブレンドモード</param>
    void Begin( CommandList* cmdList, Sprite::BlendMode defaultBlend = Sprite::BlendMode::Alpha );

    /// <summary>
    /// 描画終了
    /// </summary>
    void End();

    /// <summary>
    /// ブレンドモードからパイプラインを設定
    /// </summary>
    /// <param name="blendMode">ブレンドモード</param>
    void SetGraphicsPSO( Sprite::BlendMode blendMode );
};

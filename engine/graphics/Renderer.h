#pragma once
#include "Camera.h"
#include "SpriteBase.h"
#include "Sprite.h"

class CommandList;

/// <summary>
/// レンダラー
/// </summary>
class Renderer
{
   private:
    // スプライト基盤
    SpriteBase* mSpriteBase;

    // スプライト用カメラ
    std::unique_ptr<Camera> mSpriteCamera;

    // スプライト
    std::unique_ptr<Sprite> mOwlSprite;
    std::unique_ptr<Sprite> mStarSprite;

    // テスト
    static const uint32_t kVertexCount = 3;
    std::unique_ptr<RootSignature> mSimpleRS;
    std::unique_ptr<GraphicsPSO> mSimplePSO;
    std::unique_ptr<VertexBuffer> mSimpleVB;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static Renderer& GetInstance()
    {
        static Renderer instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Renderer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Renderer() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    Renderer( const Renderer& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    Renderer& operator=( const Renderer& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    Renderer( Renderer&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    Renderer& operator=( Renderer&& ) = delete;

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
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Draw( CommandList* cmdList );

   private:
    /// <summary>
    /// スプライトを描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void DrawSprite( CommandList* cmdList );
};

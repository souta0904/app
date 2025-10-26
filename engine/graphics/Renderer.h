#pragma once
#include "SpriteBase.h"

class CommandList;

/// <summary>
/// レンダラー
/// </summary>
class Renderer
{
   private:
    // スプライト基盤
    std::unique_ptr<SpriteBase> mSpriteBase;

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
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Draw( CommandList* cmdList );
};

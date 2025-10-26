#pragma once
#include <d3d12.h>

#include <array>
#include <memory>

#include "core/GraphicsPSO.h"
#include "core/RootSignature.h"
#include "math/Color.h"
#include "math/Matrix4.h"
#include "math/Vector2.h"
#include "math/Vector4.h"

/// <summary>
/// スプライト基盤
/// </summary>
class SpriteBase
{
   public:
    /// <summary>
    /// ブレンドモード
    /// </summary>
    enum class BlendMode
    {
        None,
        Alpha,
        Add,
        Subtract,
        Multiply,
        Screen,
        Max,
    };

   private:
    /// <summary>
    /// 頂点
    /// </summary>
    struct Vertex
    {
        Vector4 mPosition;
        Vector2 mUV;
    };

    /// <summary>
    /// 定数
    /// </summary>
    struct Constant
    {
        Matrix4 mWvp;
        Color mColor;
    };

    // ルートシグネチャ
    std::unique_ptr<RootSignature> mRS;
    // パイプラインステート
    std::array<std::unique_ptr<GraphicsPSO>, static_cast<size_t>( BlendMode::Max )> mPSO;
    // プロジェクション行列
    Matrix4 mProjectionMat;

    // コマンドリスト
    CommandList* mCmdList;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SpriteBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SpriteBase() = default;

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
    void Begin( CommandList* cmdList, BlendMode defaultBlend = BlendMode::Alpha );

    /// <summary>
    /// 描画終了
    /// </summary>
    void End();

    /// <summary>
    /// ブレンドモードからパイプラインを設定
    /// </summary>
    /// <param name="blendMode">ブレンドモード</param>
    void SetGraphicsPSO( BlendMode blendMode );
};

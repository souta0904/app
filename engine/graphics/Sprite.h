#pragma once
#include <memory>

#include "core/ConstantBuffer.h"
#include "core/VertexBuffer.h"
#include "math/Color.h"
#include "math/Matrix4.h"
#include "math/Rect.h"
#include "math/Vector2.h"

class Camera;
class CommandList;
class Texture;

/// <summary>
/// スプライト
/// </summary>
class Sprite
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
    // スプライトの頂点数
    static const uint32_t kVertexCount = 4;

    // 頂点バッファ
    std::unique_ptr<VertexBuffer> mVB;
    // 定数バッファ
    std::unique_ptr<ConstantBuffer> mCB;
    // テクスチャ
    Texture* mTexture;

   public:
    // ブレンドモード
    BlendMode mBlendMode;
    // ピボット
    Vector2 mPivot;
    // 反転フラグX
    bool mIsFlipX;
    // 反転フラグY
    bool mIsFlipY;
    // 描画矩形
    Rect mDrawRc;
    // UVトランスフォーム
    Vector2 mUVScale;
    float mUVRotate;
    Vector2 mUVTranslate;
    // 色
    Color mColor;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Sprite();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Sprite() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="texture">テクスチャ</param>
    /// <returns>成否</returns>
    bool Create( Texture* texture );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="worldMat">ワールド行列</param>
    /// <param name="camera">カメラ</param>
    void Draw( const Matrix4& worldMat, Camera* camera );

    /// <summary>テクスチャを取得</summary>
    Texture* GetTexture() const { return mTexture; }

    /// <summary>テクスチャを設定</summary>
    void SetTexture( Texture* texture );

   private:
    /// <summary>
    /// 頂点バッファを更新
    /// </summary>
    void UpdateVB();

    /// <summary>
    /// 定数バッファを更新
    /// </summary>
    /// <param name="worldMat">ワールド行列</param>
    /// <param name="camera">カメラ</param>
    void UpdateCB( const Matrix4& worldMat, Camera* camera );
};

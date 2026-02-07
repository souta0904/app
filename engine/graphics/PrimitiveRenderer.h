#pragma once
#include <memory>

#include "core/ConstantBuffer.h"
#include "core/GraphicsPSO.h"
#include "core/RootSignature.h"
#include "core/VertexBuffer.h"
#include "math/Color.h"
#include "math/Primitive.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

class Camera;
class CommandList;

/// <summary>
/// プリミティブ描画
/// </summary>
class PrimitiveRenderer
{
   private:
    static const uint32_t kMaxVertexCount = 65536;

    /// <summary>
    /// 頂点
    /// </summary>
    struct Vertex
    {
        Vector4 mPosition;
        Color mColor;
    };

    // パイプライン
    std::unique_ptr<RootSignature> mRS;
    std::unique_ptr<GraphicsPSO> mLine2D_PSO;
    std::unique_ptr<GraphicsPSO> mPrim2D_PSO;
    std::unique_ptr<GraphicsPSO> mLine3D_PSO;

    std::unique_ptr<ConstantBuffer> mCamera2D_CB;
    std::unique_ptr<ConstantBuffer> mCamera3D_CB;
    std::unique_ptr<VertexBuffer> mLine2D_VB;
    std::unique_ptr<VertexBuffer> mPrim2D_VB;
    std::unique_ptr<VertexBuffer> mLine3D_VB;

    // 頂点
    std::vector<Vertex> mLine2D_Vertices;
    std::vector<Vertex> mPrim2D_Vertices;
    std::vector<Vertex> mLine3D_Vertices;
    uint32_t mLine2D_Idx;
    uint32_t mPrim2D_Idx;
    uint32_t mLine3D_Idx;

    // カメラ
    Camera* mCamera2D;
    Camera* mCamera3D;

   public:
    /// <summary>
    /// インスタンスの取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static PrimitiveRenderer& GetInstance()
    {
        static PrimitiveRenderer instance;
        return instance;
    }

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    PrimitiveRenderer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PrimitiveRenderer() = default;

   public:
    // シングルトン
    PrimitiveRenderer( const PrimitiveRenderer& ) = delete;
    PrimitiveRenderer& operator=( const PrimitiveRenderer& ) = delete;
    PrimitiveRenderer( PrimitiveRenderer&& ) = delete;
    PrimitiveRenderer& operator=( PrimitiveRenderer&& ) = delete;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="vs">頂点シェーダ</param>
    /// <param name="ps">ピクセルシェーダ</param>
    /// <param name="camera2D">2Dカメラ</param>
    /// <param name="camera3D">3Dカメラ</param>
    /// <returns>成否</returns>
    bool Init( ShaderObject* vs, ShaderObject* ps, Camera* camera2D, Camera* camera3D );

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// ラインの描画
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="color"></param>
    void DrawLine2D( const Vector2& p1, const Vector2& p2, const Color& color );

    /// <summary>
    /// ラインの描画
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="color"></param>
    void DrawLine3D( const Vector3& p1, const Vector3& p2, const Color& color );

    /// <summary>
    /// 三角形の描画
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="color"></param>
    /// <param name="isFill"></param>
    void DrawTriangle( const Vector2& p1, const Vector2& p2, const Vector2& p3, const Color& color, bool isFill = true );

    /// <summary>
    /// 四角形の描画
    /// </summary>
    /// <param name="p1">左上</param>
    /// <param name="p2">右上</param>
    /// <param name="p3">右下</param>
    /// <param name="p4">左下</param>
    /// <param name="color"></param>
    /// <param name="isFill"></param>
    void DrawQuad( const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4, const Color& color, bool isFill = true );

    /// <summary>
    /// 長方形の描画
    /// </summary>
    /// <param name="position"></param>
    /// <param name="size"></param>
    /// <param name="color"></param>
    /// <param name="isFill"></param>
    void DrawBox( const Vector2& position, const Vector2& size, const Color& color, bool isFill = true );

    /// <summary>
    /// 2DのAABBの描画
    /// </summary>
    /// <param name="aabb"></param>
    /// <param name="color"></param>
    void DrawAABB( const AABB2D& aabb, const Color& color );

    /// <summary>
    /// 3DのAABBの描画
    /// </summary>
    /// <param name="aabb"></param>
    /// <param name="color"></param>
    void DrawAABB( const AABB3D& aabb, const Color& color );

    /// <summary>
    /// 2DのOBBの描画
    /// </summary>
    /// <param name="obb"></param>
    /// <param name="color"></param>
    void DrawOBB( const OBB2D& obb, const Color& color );

    /// <summary>
    /// 3DのOBBの描画
    /// </summary>
    /// <param name="obb"></param>
    /// <param name="color"></param>
    void DrawOBB( const OBB3D& obb, const Color& color );

    /// <summary>
    /// 円の描画
    /// </summary>
    /// <param name="circle"></param>
    /// <param name="color"></param>
    void DrawCircle( const Circle& circle, const Color& color );

    /// <summary>
    /// 球の描画
    /// </summary>
    /// <param name="sphere"></param>
    /// <param name="color"></param>
    void DrawSphere( const Sphere& sphere, const Color& color );

    /// <summary>
    /// 2Dのカプセルの描画
    /// </summary>
    /// <param name="capsule"></param>
    /// <param name="color"></param>
    void DrawCapsule( const Capsule2D& capsule, const Color& color );

    /// <summary>
    /// 3Dのカプセルの描画
    /// </summary>
    /// <param name="capsule"></param>
    /// <param name="color"></param>
    void DrawCapsule( const Capsule3D& capsule, const Color& color );

    /// <summary>
    /// 視錐台の描画
    /// </summary>
    /// <param name="vpMat"></param>
    /// <param name="color"></param>
    void DrawFrustum( const Matrix4& vpMat, const Color& color );

    /// <summary>
    /// グリッドの描画
    /// </summary>
    /// <param name="halfGridCount">片側のグリッド数</param>
    /// <param name="interval">間隔</param>
    void DrawGrid( uint32_t halfGridCount = 100, float interval = 10.0f );

    /// <summary>
    /// 2Dの描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Render2D( CommandList* cmdList );

    /// <summary>
    /// 3Dの描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Render3D( CommandList* cmdList );

    /// <summary>2Dカメラの設定</summary>
    void SetCamera2D( Camera* camera ) { mCamera2D = camera; }

    /// <summary>3Dカメラの設定</summary>
    void SetCamera3D( Camera* camera ) { mCamera3D = camera; }
};

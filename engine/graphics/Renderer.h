#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "Camera.h"
#include "DebugCamera.h"
#include "PrimitiveRenderer.h"
#include "Sprite.h"
#include "core/GraphicsPSO.h"
#include "core/RootSignature.h"
#include "light/DirectionalLight.h"
#include "light/PointLight.h"
#include "light/SpotLight.h"
#include "model/MeshSorter.h"
#include "model/ModelInstance.h"

class CommandList;
class LightManager;
class ModelBase;
class SpriteBase;

/// <summary>
/// レンダラー
/// </summary>
class Renderer
{
   private:
    // ライト管理
    LightManager* mLightManager;
    // ライト
    std::unique_ptr<DirectionalLight> mDirectionalLight;
    std::unique_ptr<PointLight> mPointLight;
    std::unique_ptr<SpotLight> mSpotLight;

    // スプライト基盤
    SpriteBase* mSpriteBase;
    // モデル基盤
    ModelBase* mModelBase;
    // プリミティブ描画
    PrimitiveRenderer* mPrimitiveRenderer;

    // カメラ
    std::unique_ptr<Camera> mSpriteCamera;
    std::unique_ptr<Camera> mModelCamera;
    bool mUseDebugCamera;
    std::unique_ptr<DebugCamera> mDebugCamera;
    // ソーター
    std::unique_ptr<MeshSorter> mSorter;

    // スプライト
    std::unique_ptr<Sprite> mOwlSprite;
    std::unique_ptr<Sprite> mStarSprite;

    // モデル
    std::unique_ptr<ModelInstance> mBotModel1;
    std::unique_ptr<ModelInstance> mBotModel2;
    std::unique_ptr<ModelInstance> mBoxModel;
    std::unique_ptr<ModelInstance> mSphereModel;
    std::unique_ptr<ModelInstance> mFloorModel;
    float mRotate;

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
    /// 入力処理
    /// </summary>
    /// <param name="state">入力状態</param>
    void Input( const InputState& state );

    /// <summary>
    /// GUIの更新
    /// </summary>
    void UpdateGUI();

    /// <summary>
    /// 更新
    /// </summary>
    void Update( float deltaTime );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Draw( CommandList* cmdList );

   private:
    /// <summary>
    /// モデルを描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void DrawModel( CommandList* cmdList );

    /// <summary>
    /// スプライトを描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void DrawSprite( CommandList* cmdList );
};

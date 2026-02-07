#include "Renderer.h"

#include "SpriteBase.h"
#include "core/CommandList.h"
#include "core/DirectXCommonSettings.h"
#include "core/ResourceManager.h"
#include "core/Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include "light/LightManager.h"
#include "model/ModelBase.h"
#include "utils/Logger.h"

// コンストラクタ
Renderer::Renderer()
    : mLightManager( nullptr )
    , mDirectionalLight( nullptr )
    , mPointLight( nullptr )
    , mSpotLight( nullptr )
    , mSpriteBase( nullptr )
    , mModelBase( nullptr )
    , mPrimitiveRenderer( nullptr )
    , mSpriteCamera( nullptr )
    , mModelCamera( nullptr )
    , mUseDebugCamera( false )
    , mDebugCamera( nullptr )
    , mSorter( nullptr )
{
}

// 初期化
bool Renderer::Init()
{
    // ライト管理を初期化
    mLightManager = &LightManager::GetInstance();
    if( !mLightManager->Init() )
    {
        LOG_ERROR( "Failed to initialize light manager." );
        MessageBox( nullptr, L"Failed to initialize light manager.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Light manager initialized successfully." );
    }

    // 平行光源
    mDirectionalLight = std::make_unique<DirectionalLight>();
    mDirectionalLight->mColor = Color::kWhite;
    mDirectionalLight->mDirection = Vector3( 1.0f, -2.0f, 3.0f );
    mDirectionalLight->mIntensity = 0.1f;
    mLightManager->AddDirectionalLight( mDirectionalLight.get() );

    // 点光源
    mPointLight = std::make_unique<PointLight>();
    mPointLight->mColor = Color( 1.0f, 1.0f, 0.0f );
    mPointLight->mPosition = Vector3( -10.0f, 10.0f, 0.0f );
    mPointLight->mIntensity = 0.8f;
    mPointLight->mRadius = 20.0f;
    mPointLight->mDecay = 2.0f;
    mLightManager->AddPointLight( mPointLight.get() );

    // スポットライト
    mSpotLight = std::make_unique<SpotLight>();
    mSpotLight->mColor = Color( 0.0f, 1.0f, 1.0f );
    mSpotLight->mPosition = Vector3( 10.0f, 20.0f, -10.0f );
    mSpotLight->mIntensity = 0.8f;
    mSpotLight->mDirection = Vector3( 0.0f, -1.0f, 1.0f );
    mSpotLight->mRadius = 60.0f;
    mSpotLight->mDecay = 2.0f;
    mSpotLight->mInnerAngle = 0.0f;
    mSpotLight->mOuterAngle = 25.0f;
    mLightManager->AddSpotLight( mSpotLight.get() );

    auto& resMgr = ResourceManager::GetInstance();
    auto* spriteVS = resMgr.GetShader( "assets/shader/SpriteVS.hlsl", "vs_6_0" );
    auto* spritePS = resMgr.GetShader( "assets/shader/SpritePS.hlsl", "ps_6_0" );

    // スプライト基盤を初期化
    mSpriteBase = &SpriteBase::GetInstance();
    if( !mSpriteBase->Init( spriteVS, spritePS ) )
    {
        LOG_ERROR( "Failed to initialize sprite base." );
        MessageBox( nullptr, L"Failed to initialize sprite base.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Sprite base initialized successfully." );
    }

    // スプライト用カメラを初期化
    mSpriteCamera = std::make_unique<Camera>();
    mSpriteCamera->mProjectionMode = Camera::ProjectionMode::Orthographic;
    mSpriteCamera->mNearZ = 0.0f;

    // スプライト
    mOwlSprite = std::make_unique<Sprite>();
    mOwlSprite->Create( resMgr.GetTexture( "assets/texture/bird_fukurou_run.png" ) );
    mStarSprite = std::make_unique<Sprite>();
    mStarSprite->Create( resMgr.GetTexture( "assets/texture/star_pattern_transparent.png" ) );
    auto& window = Window::GetInstance();
    mStarSprite->mDrawRc.right = static_cast<float>( window.GetWidth() );
    mStarSprite->mDrawRc.bottom = static_cast<float>( window.GetHeight() );

    // ルートシグネチャを作成
    mSimpleRS = std::make_unique<RootSignature>();
    mSimpleRS->Init( 0, 0 );
    if( !mSimpleRS->Create() )
    {
        return false;
    }

    // パイプラインステートを作成
    PSOInit init = {};
    init.mRootSignature = mSimpleRS.get();
    init.mVS = resMgr.GetShader( "assets/shader/SimpleVS.hlsl", "vs_6_0" );
    init.mPS = resMgr.GetShader( "assets/shader/SimplePS.hlsl", "ps_6_0" );
    init.mBlendState = DirectXCommonSettings::gBlendAlpha;
    init.mRasterizerState = DirectXCommonSettings::gRasterizerDefault;
    init.mDepthStencilState = DirectXCommonSettings::gDepthDisable;
    init.mInputLayouts.resize( 2 );
    init.mInputLayouts[0].SemanticName = "POSITION";
    init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mInputLayouts[1].SemanticName = "COLOR";
    init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    mSimplePSO = std::make_unique<GraphicsPSO>();
    if( !mSimplePSO->Create( init ) )
    {
        return false;
    }

    // 頂点を作成
    struct Vertex
    {
        Vector4 mPosition;
        Color mColor;
    };
    mSimpleVB = std::make_unique<VertexBuffer>();
    if( !mSimpleVB->Create( sizeof( Vertex ) * kVertexCount, sizeof( Vertex ) ) )
    {
        return false;
    }
    Vertex v[kVertexCount] =
        {
            { Vector4( -0.3f, -0.5f, 0.0f, 1.0f ), Color::kRed },
            { Vector4( +0.0f, +0.5f, 0.0f, 1.0f ), Color::kGreen },
            { Vector4( +0.3f, -0.5f, 0.0f, 1.0f ), Color::kBlue },
        };
    mSimpleVB->Update( v );

    // モデル基盤を初期化
    mModelBase = &ModelBase::GetInstance();
    if( !mModelBase->Init() )
    {
        LOG_ERROR( "Failed to initialize model base." );
        MessageBox( nullptr, L"Failed to initialize model base.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Model base initialized successfully." );
    }

    mModelCamera = std::make_unique<Camera>();
    mModelCamera->mPosition = Vector3( 0.0f, 10.0f, -50.0f );
    mModelCamera->mNearZ = 10.0f;
    mModelCamera->mFarZ = 250.0f;

    mDebugCamera = std::make_unique<DebugCamera>();

    // ソーターの初期化
    mSorter = std::make_unique<MeshSorter>();
    if( !mSorter->Init( mModelCamera.get() ) )
    {
        return false;
    }
    mSorter->SetFrustumCamera( mModelCamera.get() );

    mBotModel1 = std::make_unique<ModelInstance>();
    mBotModel1->Create( resMgr.GetModel( "assets/model/bot/y_bot.fbx" ) );

    mBotModel2 = std::make_unique<ModelInstance>();
    mBotModel2->Create( resMgr.GetModel( "assets/model/bot/x_bot.fbx" ) );
    mRotate = 0.0f;

    mBoxModel = std::make_unique<ModelInstance>();
    mBoxModel->Create( resMgr.GetModel( "assets/model/box/box.obj" ) );

    mSphereModel = std::make_unique<ModelInstance>();
    mSphereModel->Create( resMgr.GetModel( "assets/model/sphere/sphere.obj" ) );

    mFloorModel = std::make_unique<ModelInstance>();
    mFloorModel->Create( resMgr.GetModel( "assets/model/floor/floor.glb" ) );

    auto* primitiveVS = resMgr.GetShader( "assets/shader/PrimitiveVS.hlsl", "vs_6_0" );
    auto* primitivePS = resMgr.GetShader( "assets/shader/PrimitivePS.hlsl", "ps_6_0" );

    // プリミティブ描画の初期化
    mPrimitiveRenderer = &PrimitiveRenderer::GetInstance();
    if( !mPrimitiveRenderer->Init( primitiveVS, primitivePS, mSpriteCamera.get(), mModelCamera.get() ) )
    {
        LOG_ERROR( "Failed to initialize primitive renderer." );
        MessageBox( nullptr, L"Failed to initialize primitive renderer.", L"Error", MB_OK | MB_ICONERROR );
        return false;
    }
    else
    {
        LOG_INFO( "Primitive renderer initialized successfully." );
    }

    return true;
}

// 終了処理
void Renderer::Term()
{
    if( mModelBase )
    {
        mModelBase->Term();
        mModelBase = nullptr;
        LOG_INFO( "Model base terminated." );
    }
    if( mSpriteBase )
    {
        mSpriteBase->Term();
        mSpriteBase = nullptr;
        LOG_INFO( "Sprite base terminated." );
    }
    if( mSpotLight )
    {
        mLightManager->RemoveSpotLight( mSpotLight.get() );
    }
    if( mPointLight )
    {
        mLightManager->RemovePointLight( mPointLight.get() );
    }
    if( mDirectionalLight )
    {
        mLightManager->RemoveDirectionalLight( mDirectionalLight.get() );
    }
    if( mLightManager )
    {
        mLightManager->Term();
        mLightManager = nullptr;
        LOG_INFO( "Light manager terminated." );
    }
}

// 入力処理
void Renderer::Input( const InputState& state )
{
    if( mUseDebugCamera )
    {
        mDebugCamera->Input( state );
    }
}

// GUIの更新
void Renderer::UpdateGUI()
{
    ImGui::Begin( "Renderer" );

    // カメラ
    ImGui::SetNextItemOpen( true, ImGuiCond_Once );
    if( ImGui::TreeNode( "Camera" ) )
    {
        ImGui::DragFloat3( "Position", &mModelCamera->mPosition.x, 0.01f );
        ImGui::Checkbox( "Use Debug Camera", &mUseDebugCamera );
        ImGui::TreePop();
    }

    // 平行光源
    ImGui::SetNextItemOpen( true, ImGuiCond_Once );
    if( ImGui::TreeNode( "Directional Light" ) )
    {
        ImGui::ColorEdit3( "Color", &mDirectionalLight->mColor.r );
        ImGui::DragFloat3( "Direction", &mDirectionalLight->mDirection.x, 0.01f );
        ImGui::DragFloat( "Intensity", &mDirectionalLight->mIntensity, 0.01f, 0.0f, FLT_MAX );
        ImGui::TreePop();
    }

    // 点光源
    ImGui::SetNextItemOpen( true, ImGuiCond_Once );
    if( ImGui::TreeNode( "Point Light" ) )
    {
        ImGui::ColorEdit3( "Color", &mPointLight->mColor.r );
        ImGui::DragFloat3( "Position", &mPointLight->mPosition.x, 0.01f );
        ImGui::DragFloat( "Intensity", &mPointLight->mIntensity, 0.01f, 0.0f, FLT_MAX );
        ImGui::DragFloat( "Radius", &mPointLight->mRadius, 0.01f, 0.0f, FLT_MAX );
        ImGui::DragFloat( "Decay", &mPointLight->mDecay, 0.01f, 0.0f, FLT_MAX );
        ImGui::TreePop();
    }

    // スポットライト
    ImGui::SetNextItemOpen( true, ImGuiCond_Once );
    if( ImGui::TreeNode( "Spot Light" ) )
    {
        ImGui::ColorEdit3( "Color", &mSpotLight->mColor.r );
        ImGui::DragFloat3( "Position", &mSpotLight->mPosition.x, 0.01f );
        ImGui::DragFloat( "Intensity", &mSpotLight->mIntensity, 0.01f, 0.0f, FLT_MAX );
        ImGui::DragFloat3( "Direction", &mSpotLight->mDirection.x, 0.01f );
        ImGui::DragFloat( "Radius", &mSpotLight->mRadius, 0.01f, 0.0f, FLT_MAX );
        ImGui::DragFloat( "Decay", &mSpotLight->mDecay, 0.01f, 0.0f, FLT_MAX );
        ImGui::DragFloat( "Inner Angle", &mSpotLight->mInnerAngle, 0.01f, 0.0f, mSpotLight->mOuterAngle );
        ImGui::DragFloat( "Outer Angle", &mSpotLight->mOuterAngle, 0.01f, mSpotLight->mInnerAngle, 180.0f );
        ImGui::TreePop();
    }

    ImGui::End();
}

// 更新
void Renderer::Update( float deltaTime )
{
    mSpriteCamera->Update();
    mModelCamera->Update();

    if( !mUseDebugCamera )
    {
        mSorter->SetCamera( mModelCamera.get() );
        mPrimitiveRenderer->SetCamera3D( mModelCamera.get() );
    }
    else
    {
        mDebugCamera->Update();
        mSorter->SetCamera( mDebugCamera->GetCamera() );
        mPrimitiveRenderer->SetCamera3D( mDebugCamera->GetCamera() );
    }

    // UVスクロール
    mStarSprite->mUVTranslate += Vector2( 0.1f * deltaTime, -0.1f * deltaTime );

    mRotate += MathUtil::kPi * deltaTime;

    mBotModel1->Update( deltaTime );
    mBotModel2->Update( deltaTime );
    mBoxModel->Update( deltaTime );
    mSphereModel->Update( deltaTime );
    mFloorModel->Update( deltaTime );
}

// 描画
void Renderer::Draw( CommandList* cmdList )
{
    auto& window = Window::GetInstance();
    auto windowWidth = static_cast<float>( window.GetWidth() );
    auto windowHeight = static_cast<float>( window.GetHeight() );
    cmdList->SetViewport( 0.0f, 0.0f, windowWidth, windowHeight );
    cmdList->SetScissorRect( 0.0f, 0.0f, windowWidth, windowHeight );

    DrawSprite( cmdList );

    cmdList->SetGraphicsRootSignature( mSimpleRS.get() );
    cmdList->SetPipelineState( mSimplePSO.get() );
    cmdList->SetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    cmdList->SetVertexBuffer( mSimpleVB.get() );
    cmdList->DrawInstanced( kVertexCount );

    DrawModel( cmdList );

    /*
    mPrimitiveRenderer->DrawLine2D( Vector2( 150.0f, 150.0f ), Vector2( 50.0f, 50.0f ), Color::kWhite );
    mPrimitiveRenderer->DrawTriangle( Vector2( 200.0f, 150.0f ), Vector2( 300.0f, 50.0f ), Vector2( 300.0f, 150.0f ), Color::kRed );
    mPrimitiveRenderer->DrawQuad( Vector2( 350.0f, 50.0f ), Vector2( 450.0f, 50.0f ), Vector2( 500.0f, 150.0f ), Vector2( 400.0f, 150.0f ), Color::kGreen );
    mPrimitiveRenderer->DrawBox( Vector2( 550.0f, 50.0f ), Vector2( 100.0f, 100.0f ), Color::kBlue );

    AABB2D aabb2D = {};
    aabb2D.mMin = Vector2( 700.0f, 50.0f );
    aabb2D.mMax = Vector2( 800.0f, 150.0f );
    mPrimitiveRenderer->DrawAABB( aabb2D, Color::kYellow );

    OBB2D obb2D = {};
    obb2D.mCenter = Vector2( 900.0f, 100.0f );
    obb2D.mHalfSize = Vector2( 50.0f, 25.0f );
    auto rad = -20.0f * MathUtil::kDegToRad;
    auto c = std::cos( rad );
    auto s = std::sin( rad );
    obb2D.mAxes[0] = Normalize( Vector2( c, s ) );
    obb2D.mAxes[1] = Normalize( Vector2( -s, c ) );
    mPrimitiveRenderer->DrawOBB( obb2D, Color::kCyan );

    Circle circle = {};
    circle.mCenter = Vector2( 1050.0f, 100.0f );
    circle.mRadius = 50.0f;
    mPrimitiveRenderer->DrawCircle( circle, Color::kMagenta );

    Capsule2D capsule2D = {};
    capsule2D.mSegment.mStart = Vector2( 1170.0f, 70.0f );
    capsule2D.mSegment.mEnd = Vector2( 1230.0f, 130.0f );
    capsule2D.mRadius = 20.0f;
    mPrimitiveRenderer->DrawCapsule( capsule2D, Color::kWhite );

    AABB3D aabb3D = {};
    aabb3D.mMin = Vector3( -23.0f, 2.0f, 12.0f );
    aabb3D.mMax = Vector3( -17.0f, 8.0f, 18.0f );
    mPrimitiveRenderer->DrawAABB( aabb3D, Color::kRed );

    OBB3D obb3D = {};
    obb3D.mCenter = Vector3( -7.5f, 5.0f, 15.0f );
    obb3D.mHalfSize = Vector3( 2.5f, 2.5f, 7.5f );
    c = std::cos( rad );
    s = std::sin( rad );
    obb3D.mAxes[0] = Normalize( Vector3( c, 0.0f, -s ) );
    obb3D.mAxes[1] = Vector3::kUnitY;
    obb3D.mAxes[2] = Normalize( Vector3( s, 0.0f, c ) );
    mPrimitiveRenderer->DrawOBB( obb3D, Color::kGreen );

    Sphere sphere = {};
    sphere.mCenter = Vector3( 5.0f, 5.0f, 15.0f );
    sphere.mRadius = 5.0f;
    mPrimitiveRenderer->DrawSphere( sphere, Color::kBlue );

    Capsule3D capsule3D = {};
    capsule3D.mSegment.mStart = Vector3( 18.0f, 3.0f,13.0f );
    capsule3D.mSegment.mEnd = Vector3( 22.0f, 7.0f, 17.0f );
    capsule3D.mRadius = 3.0f;
    mPrimitiveRenderer->DrawCapsule( capsule3D, Color::kYellow );
    */

    mPrimitiveRenderer->DrawFrustum( mModelCamera->GetView() * mModelCamera->GetProjection(), Color::kWhite );
    mPrimitiveRenderer->DrawGrid();
    mPrimitiveRenderer->Render3D( cmdList );
    mPrimitiveRenderer->Render2D( cmdList );
}

// モデルを描画
void Renderer::DrawModel( CommandList* cmdList )
{
    if( !mModelBase ) return;

    mModelBase->Begin( cmdList );

    mLightManager->Bind( cmdList, 4 );

    mFloorModel->Draw( mSorter.get(), Matrix4() );

    auto botWorld1 =
        CreateScale( Vector3::kOne * 0.1f ) *
        CreateRotate( Quaternion( Vector3::kUnitY, MathUtil::kPi ) ) *
        CreateTranslate( Vector3( 10.0f, 0.0f, 0.0f ) );
    mBotModel1->Draw( mSorter.get(), botWorld1 );

    auto botWorld2 =
        CreateScale( Vector3::kOne * 0.1f ) *
        CreateRotate( Quaternion( Vector3::kUnitY, MathUtil::kPi ) ) *
        CreateTranslate( Vector3( -10.0f, 0.0f, 0.0f ) );
    mBotModel2->Draw( mSorter.get(), botWorld2 );

    auto boxWorld =
        CreateRotate( Quaternion( Vector3::kUnitY, mRotate ) ) *
        CreateTranslate( Vector3( 2.5f, 0.0f, 0.0f ) );
    mBoxModel->Draw( mSorter.get(), boxWorld );

    auto sphereWorld =
        CreateRotate( Quaternion( Vector3::kUnitY, mRotate ) ) *
        CreateTranslate( Vector3( -2.5f, 0.0f, 0.0f ) );
    mSphereModel->Draw( mSorter.get(), sphereWorld );

    mSorter->Sort();

    mSorter->Render( cmdList );

    mModelBase->End();
}

// スプライトを描画
void Renderer::DrawSprite( CommandList* cmdList )
{
    if( !mSpriteBase ) return;

    mSpriteBase->Begin( cmdList );

    mStarSprite->Draw( Matrix4(), mSpriteCamera.get() );
    mOwlSprite->Draw( Matrix4(), mSpriteCamera.get() );

    mSpriteBase->End();
}

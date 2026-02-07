#include "PrimitiveRenderer.h"

#include "core/CommandList.h"
#include "core/DirectXCommonSettings.h"
#include "graphics/Camera.h"

// コンストラクタ
PrimitiveRenderer::PrimitiveRenderer()
    : mRS( nullptr )
    , mLine2D_PSO( nullptr )
    , mPrim2D_PSO( nullptr )
    , mLine3D_PSO( nullptr )
    , mCamera2D_CB( nullptr )
    , mCamera3D_CB( nullptr )
    , mLine2D_VB( nullptr )
    , mPrim2D_VB( nullptr )
    , mLine3D_VB( nullptr )
    , mLine2D_Vertices()
    , mPrim2D_Vertices()
    , mLine3D_Vertices()
    , mLine2D_Idx( 0 )
    , mPrim2D_Idx( 0 )
    , mLine3D_Idx( 0 )
    , mCamera2D( nullptr )
    , mCamera3D( nullptr )
{
}

// 初期化
bool PrimitiveRenderer::Init( ShaderObject* vs, ShaderObject* ps, Camera* camera2D, Camera* camera3D )
{
    if( !vs || !ps ) return false;

    // ルートシグネチャの作成
    mRS = std::make_unique<RootSignature>();
    mRS->Init( 1, 0 );
    mRS->GetParameter( 0 ).InitAsCBV( 0 );
    if( !mRS->Create() )
    {
        return false;
    }

    // パイプラインステートの作成
    PSOInit init = {};
    init.mRootSignature = mRS.get();
    init.mVS = vs;
    init.mPS = ps;
    init.mBlendState = DirectXCommonSettings::gBlendAlpha;
    init.mRasterizerState = DirectXCommonSettings::gRasterizerNoCulling;
    init.mDepthStencilState = DirectXCommonSettings::gDepthDisable;
    init.mInputLayouts.resize( 2 );
    init.mInputLayouts[0].SemanticName = "POSITION";
    init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mInputLayouts[1].SemanticName = "COLOR";
    init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    init.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    mLine2D_PSO = std::make_unique<GraphicsPSO>();
    if( !mLine2D_PSO->Create( init ) )
    {
        return false;
    }
    init.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    mPrim2D_PSO = std::make_unique<GraphicsPSO>();
    if( !mPrim2D_PSO->Create( init ) )
    {
        return false;
    }
    init.mDepthStencilState = DirectXCommonSettings::gDepthDefault;
    init.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    mLine3D_PSO = std::make_unique<GraphicsPSO>();
    if( !mLine3D_PSO->Create( init ) )
    {
        return false;
    }

    mCamera2D_CB = std::make_unique<ConstantBuffer>();
    mCamera2D_CB->Create( sizeof( Matrix4 ) );
    mCamera3D_CB = std::make_unique<ConstantBuffer>();
    mCamera3D_CB->Create( sizeof( Matrix4 ) );
    mLine2D_VB = std::make_unique<VertexBuffer>();
    mLine2D_VB->Create( sizeof( Vertex ) * kMaxVertexCount, sizeof( Vertex ) );
    mPrim2D_VB = std::make_unique<VertexBuffer>();
    mPrim2D_VB->Create( sizeof( Vertex ) * kMaxVertexCount, sizeof( Vertex ) );
    mLine3D_VB = std::make_unique<VertexBuffer>();
    mLine3D_VB->Create( sizeof( Vertex ) * kMaxVertexCount, sizeof( Vertex ) );

    mLine2D_Vertices.resize( kMaxVertexCount );
    mPrim2D_Vertices.resize( kMaxVertexCount );
    mLine3D_Vertices.resize( kMaxVertexCount );
    mLine2D_Idx = 0;
    mPrim2D_Idx = 0;
    mLine3D_Idx = 0;

    mCamera2D = camera2D;
    mCamera3D = camera3D;

    return true;
}

void PrimitiveRenderer::Term()
{
}

// ラインの描画
void PrimitiveRenderer::DrawLine2D( const Vector2& p1, const Vector2& p2, const Color& color )
{
    mLine2D_Vertices[mLine2D_Idx++] = { Vector4( p1.x, p1.y, 0.0f, 1.0f ), color };
    mLine2D_Vertices[mLine2D_Idx++] = { Vector4( p2.x, p2.y, 0.0f, 1.0f ), color };
}

// ラインの描画
void PrimitiveRenderer::DrawLine3D( const Vector3& p1, const Vector3& p2, const Color& color )
{
    mLine3D_Vertices[mLine3D_Idx++] = { Vector4( p1.x, p1.y, p1.z, 1.0f ), color };
    mLine3D_Vertices[mLine3D_Idx++] = { Vector4( p2.x, p2.y, p2.z, 1.0f ), color };
}

// 三角形の描画
void PrimitiveRenderer::DrawTriangle( const Vector2& p1, const Vector2& p2, const Vector2& p3, const Color& color, bool isFill )
{
    if( isFill )
    {
        mPrim2D_Vertices[mPrim2D_Idx++] = { Vector4( p1.x, p1.y, 0.0f, 1.0f ), color };
        mPrim2D_Vertices[mPrim2D_Idx++] = { Vector4( p2.x, p2.y, 0.0f, 1.0f ), color };
        mPrim2D_Vertices[mPrim2D_Idx++] = { Vector4( p3.x, p3.y, 0.0f, 1.0f ), color };
    }
    else
    {
        DrawLine2D( p1, p2, color );
        DrawLine2D( p2, p3, color );
        DrawLine2D( p3, p1, color );
    }
}

// 四角形の描画
void PrimitiveRenderer::DrawQuad( const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4, const Color& color, bool isFill )
{
    if( isFill )
    {
        DrawTriangle( p1, p2, p3, color, true );
        DrawTriangle( p3, p4, p1, color, true );
    }
    else
    {
        DrawLine2D( p1, p2, color );
        DrawLine2D( p2, p3, color );
        DrawLine2D( p3, p4, color );
        DrawLine2D( p4, p1, color );
    }
}

// 長方形の描画
void PrimitiveRenderer::DrawBox( const Vector2& position, const Vector2& size, const Color& color, bool isFill )
{
    DrawQuad(
        position,
        position + Vector2( size.x, 0.0f ),
        position + size,
        position + Vector2( 0.0f, size.y ),
        color,
        isFill );
}

// 2DのAABBの描画
void PrimitiveRenderer::DrawAABB( const AABB2D& aabb, const Color& color )
{
    DrawBox( aabb.mMin, aabb.mMax - aabb.mMin, color, false );
}

// 3DのAABBの描画
void PrimitiveRenderer::DrawAABB( const AABB3D& aabb, const Color& color )
{
    const auto& min = aabb.mMin;
    const auto& max = aabb.mMax;

    Vector3 v[8] = {};
    v[0] = min;
    v[1] = Vector3( max.x, min.y, min.z );
    v[2] = Vector3( max.x, min.y, max.z );
    v[3] = Vector3( min.x, min.y, max.z );
    v[4] = Vector3( min.x, max.y, min.z );
    v[5] = Vector3( max.x, max.y, min.z );
    v[6] = max;
    v[7] = Vector3( min.x, max.y, max.z );

    DrawLine3D( v[0], v[1], color );
    DrawLine3D( v[1], v[2], color );
    DrawLine3D( v[2], v[3], color );
    DrawLine3D( v[3], v[0], color );

    DrawLine3D( v[4], v[5], color );
    DrawLine3D( v[5], v[6], color );
    DrawLine3D( v[6], v[7], color );
    DrawLine3D( v[7], v[4], color );

    DrawLine3D( v[0], v[4], color );
    DrawLine3D( v[1], v[5], color );
    DrawLine3D( v[2], v[6], color );
    DrawLine3D( v[3], v[7], color );
}

// 2DのOBBの描画
void PrimitiveRenderer::DrawOBB( const OBB2D& obb, const Color& color )
{
    auto ax = obb.mAxes[0] * obb.mHalfSize.x;
    auto ay = obb.mAxes[1] * obb.mHalfSize.y;

    Vector2 v[4] = {};
    v[0] = obb.mCenter - ax - ay;
    v[1] = obb.mCenter + ax - ay;
    v[2] = obb.mCenter + ax + ay;
    v[3] = obb.mCenter - ax + ay;

    DrawLine2D( v[0], v[1], color );
    DrawLine2D( v[1], v[2], color );
    DrawLine2D( v[2], v[3], color );
    DrawLine2D( v[3], v[0], color );
}

// 3DのOBBの描画
void PrimitiveRenderer::DrawOBB( const OBB3D& obb, const Color& color )
{
    auto ax = obb.mAxes[0] * obb.mHalfSize.x;
    auto ay = obb.mAxes[1] * obb.mHalfSize.y;
    auto az = obb.mAxes[2] * obb.mHalfSize.z;

    Vector3 v[8] = {};
    v[0] = obb.mCenter - ax - ay - az;
    v[1] = obb.mCenter + ax - ay - az;
    v[2] = obb.mCenter + ax - ay + az;
    v[3] = obb.mCenter - ax - ay + az;
    v[4] = obb.mCenter - ax + ay - az;
    v[5] = obb.mCenter + ax + ay - az;
    v[6] = obb.mCenter + ax + ay + az;
    v[7] = obb.mCenter - ax + ay + az;

    DrawLine3D( v[0], v[1], color );
    DrawLine3D( v[1], v[2], color );
    DrawLine3D( v[2], v[3], color );
    DrawLine3D( v[3], v[0], color );

    DrawLine3D( v[4], v[5], color );
    DrawLine3D( v[5], v[6], color );
    DrawLine3D( v[6], v[7], color );
    DrawLine3D( v[7], v[4], color );

    DrawLine3D( v[0], v[4], color );
    DrawLine3D( v[1], v[5], color );
    DrawLine3D( v[2], v[6], color );
    DrawLine3D( v[3], v[7], color );
}

// 円の描画
void PrimitiveRenderer::DrawCircle( const Circle& circle, const Color& color )
{
    const auto kLineCnt = 32;
    auto step = MathUtil::kTwoPi / kLineCnt;
    auto prev = circle.mCenter + Vector2( 1.0f, 0.0f ) * circle.mRadius;
    for( auto i = 1; i <= kLineCnt; ++i )
    {
        auto t = step * i;
        auto curr = circle.mCenter + Vector2( std::cos( t ), std::sin( t ) ) * circle.mRadius;
        DrawLine2D( prev, curr, color );
        prev = curr;
    }
}

// 球の描画
void PrimitiveRenderer::DrawSphere( const Sphere& sphere, const Color& color )
{
    const auto kLineCnt = 32;
    auto step = MathUtil::kTwoPi / kLineCnt;
    // yz平面
    auto prev = sphere.mCenter + Vector3( 0.0f, 1.0f, 0.0f ) * sphere.mRadius;
    for( auto i = 1; i <= kLineCnt; ++i )
    {
        auto t = step * i;
        auto curr = sphere.mCenter + Vector3( 0.0f, std::cos( t ), std::sin( t ) ) * sphere.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
    // xz平面
    prev = sphere.mCenter + Vector3( 1.0f, 0.0f, 0.0f ) * sphere.mRadius;
    for( auto i = 1; i <= kLineCnt; ++i )
    {
        auto t = step * i;
        auto curr = sphere.mCenter + Vector3( std::cos( t ), 0.0f, std::sin( t ) ) * sphere.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
    // xy平面
    prev = sphere.mCenter + Vector3( 1.0f, 0.0f, 0.0f ) * sphere.mRadius;
    for( auto i = 1; i <= kLineCnt; ++i )
    {
        auto t = step * i;
        auto curr = sphere.mCenter + Vector3( std::cos( t ), std::sin( t ), 0.0f ) * sphere.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
}

// 2Dのカプセルの描画
void PrimitiveRenderer::DrawCapsule( const Capsule2D& capsule, const Color& color )
{
    auto start = capsule.mSegment.mStart;
    auto end = capsule.mSegment.mEnd;
    auto v = Normalize( end - start );
    auto n = Vector2( -v.y, v.x );
    DrawLine2D( start + n * capsule.mRadius, end + n * capsule.mRadius, color );
    DrawLine2D( start - n * capsule.mRadius, end - n * capsule.mRadius, color );

    // DrawCircle( Circle{ start, capsule.mRadius }, color );
    // DrawCircle( Circle{ end, capsule.mRadius }, color );

    const auto kLineCnt = 32;
    auto step = MathUtil::kTwoPi / kLineCnt;

    // 半円
    auto prev = start + n * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt / 2; ++i )
    {
        auto t = step * i;
        auto curr = start + ( n * std::cos( t ) - v * std::sin( t ) ) * capsule.mRadius;
        DrawLine2D( prev, curr, color );
        prev = curr;
    }
    prev = end + n * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt / 2; ++i )
    {
        auto t = step * i;
        auto curr = end + ( n * std::cos( t ) + v * std::sin( t ) ) * capsule.mRadius;
        DrawLine2D( prev, curr, color );
        prev = curr;
    }
}

// 3Dのカプセルの描画
void PrimitiveRenderer::DrawCapsule( const Capsule3D& capsule, const Color& color )
{
    auto start = capsule.mSegment.mStart;
    auto end = capsule.mSegment.mEnd;
    auto forward = Normalize( end - start );
    auto tmpUp = std::fabs( forward.y ) < 0.999f ? Vector3::kUnitY : Vector3::kUnitX;
    auto right = Normalize( Cross( tmpUp, forward ) );
    auto up = Normalize( Cross( forward, right ) );
    DrawLine3D( start + right * capsule.mRadius, end + right * capsule.mRadius, color );
    DrawLine3D( start - right * capsule.mRadius, end - right * capsule.mRadius, color );
    DrawLine3D( start + up * capsule.mRadius, end + up * capsule.mRadius, color );
    DrawLine3D( start - up * capsule.mRadius, end - up * capsule.mRadius, color );

    // DrawSphere( Sphere{ start, capsule.mRadius }, color );
    // DrawSphere( Sphere{ end, capsule.mRadius }, color );

    const auto kLineCnt = 32;
    auto step = MathUtil::kTwoPi / kLineCnt;

    // 始点の円
    // 円
    auto prev = start + right * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt; ++i )
    {
        auto t = step * i;
        auto curr = start + ( right * std::cos( t ) + up * std::sin( t ) ) * capsule.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
    // 半円
    prev = start + right * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt / 2; ++i )
    {
        auto t = step * i;
        auto curr = start + ( right * std::cos( t ) - forward * std::sin( t ) ) * capsule.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
    // 半円
    prev = start + up * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt / 2; ++i )
    {
        auto t = step * i;
        auto curr = start + ( up * std::cos( t ) - forward * std::sin( t ) ) * capsule.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }

    // 終点の円
    // 円
    prev = end + right * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt; ++i )
    {
        auto t = step * i;
        auto curr = end + ( right * std::cos( t ) + up * std::sin( t ) ) * capsule.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
    // 半円
    prev = end + right * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt / 2; ++i )
    {
        auto t = step * i;
        auto curr = end + ( right * std::cos( t ) + forward * std::sin( t ) ) * capsule.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
    // 半円
    prev = end + up * capsule.mRadius;
    for( auto i = 1; i <= kLineCnt / 2; ++i )
    {
        auto t = step * i;
        auto curr = end + ( up * std::cos( t ) + forward * std::sin( t ) ) * capsule.mRadius;
        DrawLine3D( prev, curr, color );
        prev = curr;
    }
}

// 視錐台の描画
void PrimitiveRenderer::DrawFrustum( const Matrix4& vpMat, const Color& color )
{
    Vector3 v[8] = {};
    v[0] = Vector3( -1.0f, -1.0f, 0.0f );
    v[1] = Vector3( +1.0f, -1.0f, 0.0f );
    v[2] = Vector3( +1.0f, +1.0f, 0.0f );
    v[3] = Vector3( -1.0f, +1.0f, 0.0f );
    v[4] = Vector3( -1.0f, -1.0f, 1.0f );
    v[5] = Vector3( +1.0f, -1.0f, 1.0f );
    v[6] = Vector3( +1.0f, +1.0f, 1.0f );
    v[7] = Vector3( -1.0f, +1.0f, 1.0f );

    auto invMat = Inverse( vpMat );
    for (uint32_t i = 0; i < 8; ++i)
    {
        auto p = Vector4( v[i].x, v[i].y, v[i].z, 1.0f ) * invMat;
        v[i] = Vector3( p.x, p.y, p.z ) / p.w;
    }

    DrawLine3D( v[0], v[1], color );
    DrawLine3D( v[1], v[2], color );
    DrawLine3D( v[2], v[3], color );
    DrawLine3D( v[3], v[0], color );

    DrawLine3D( v[4], v[5], color );
    DrawLine3D( v[5], v[6], color );
    DrawLine3D( v[6], v[7], color );
    DrawLine3D( v[7], v[4], color );

    DrawLine3D( v[0], v[4], color );
    DrawLine3D( v[1], v[5], color );
    DrawLine3D( v[2], v[6], color );
    DrawLine3D( v[3], v[7], color );
}

// グリッドの描画
void PrimitiveRenderer::DrawGrid( uint32_t halfGridCount, float interval )
{
    auto lineCnt = static_cast<int32_t>( halfGridCount * 2 + 1 );
    auto halfExtent = ( lineCnt - 1 ) * interval / 2.0f;
    auto y = 0.05f;
    auto color = Color( 1.0f, 1.0f, 1.0f, 0.05f );
    // 横
    for( auto i = 0; i < lineCnt; ++i )
    {
        auto idx = i - static_cast<int32_t>( halfGridCount );
        if( idx == 0 ) continue;  // 中心は最後

        auto z = idx * interval;
        mLine3D_Vertices[mLine3D_Idx++] = { Vector4( -halfExtent, y, z, 1.0f ), color };
        mLine3D_Vertices[mLine3D_Idx++] = { Vector4( +halfExtent, y, z, 1.0f ), color };
    }
    // 縦
    for( auto i = 0; i < lineCnt; ++i )
    {
        auto idx = i - static_cast<int32_t>( halfGridCount );
        if( idx == 0 ) continue;  // 中心は最後

        auto x = idx * interval;
        mLine3D_Vertices[mLine3D_Idx++] = { Vector4( x, y, -halfExtent, 1.0f ), color };
        mLine3D_Vertices[mLine3D_Idx++] = { Vector4( x, y, +halfExtent, 1.0f ), color };
    }
    // 中心
    auto zAxisColor = Color( 0.2f, 0.2f, 1.0f, 1.0f );
    mLine3D_Vertices[mLine3D_Idx++] = { Vector4( 0.0f, y, -halfExtent, 1.0f ), zAxisColor };
    mLine3D_Vertices[mLine3D_Idx++] = { Vector4( 0.0f, y, +halfExtent, 1.0f ), zAxisColor };
    auto xAxisColor = Color( 1.0f, 0.2f, 0.2f, 1.0f );
    mLine3D_Vertices[mLine3D_Idx++] = { Vector4( -halfExtent, y, 0.0f, 1.0f ), xAxisColor };
    mLine3D_Vertices[mLine3D_Idx++] = { Vector4( +halfExtent, y, 0.0f, 1.0f ), xAxisColor };
}

// 2Dの描画
void PrimitiveRenderer::Render2D( CommandList* cmdList )
{
    if( !mCamera2D ) return;

    auto vpMat = mCamera2D->GetView() * mCamera2D->GetProjection();
    mCamera2D_CB->Update( &vpMat );
    mLine2D_VB->Update( mLine2D_Vertices.data() );
    mPrim2D_VB->Update( mPrim2D_Vertices.data() );

    cmdList->SetGraphicsRootSignature( mRS.get() );
    cmdList->SetConstantBuffer( 0, mCamera2D_CB.get() );

    cmdList->SetPipelineState( mLine2D_PSO.get() );
    cmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINELIST );
    cmdList->SetVertexBuffer( mLine2D_VB.get() );
    cmdList->DrawInstanced( mLine2D_Idx );
    mLine2D_Idx = 0;

    cmdList->SetPipelineState( mPrim2D_PSO.get() );
    cmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    cmdList->SetVertexBuffer( mPrim2D_VB.get() );
    cmdList->DrawInstanced( mPrim2D_Idx );
    mPrim2D_Idx = 0;
}

// 3Dの描画
void PrimitiveRenderer::Render3D( CommandList* cmdList )
{
    if( !mCamera3D ) return;

    auto vpMat = mCamera3D->GetView() * mCamera3D->GetProjection();
    mCamera3D_CB->Update( &vpMat );
    mLine3D_VB->Update( mLine3D_Vertices.data() );

    cmdList->SetGraphicsRootSignature( mRS.get() );
    cmdList->SetPipelineState( mLine3D_PSO.get() );
    cmdList->SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINELIST );
    cmdList->SetConstantBuffer( 0, mCamera3D_CB.get() );
    cmdList->SetVertexBuffer( mLine3D_VB.get() );
    cmdList->DrawInstanced( mLine3D_Idx );
    mLine3D_Idx = 0;
}

#include "Sprite.h"

#include "Camera.h"
#include "SpriteBase.h"
#include "Texture.h"
#include "core/CommandList.h"

// コンストラクタ
Sprite::Sprite()
    : mVB( nullptr )
    , mCB( nullptr )
    , mTexture( nullptr )
    , mBlendMode( BlendMode::Alpha )
    , mPivot( Vector2::kZero )
    , mIsFlipX( false )
    , mIsFlipY( false )
    , mDrawRc()
    , mUVScale( Vector2::kOne )
    , mUVRotate( 0.0f )
    , mUVTranslate( Vector2::kZero )
    , mColor( Color::kWhite )
{
}

// 作成
bool Sprite::Create( Texture* texture )
{
    SetTexture( texture );

    // 頂点バッファを作成
    mVB = std::make_unique<VertexBuffer>();
    if( !mVB->Create( sizeof( SpriteBase::Vertex ) * kVertexCount, sizeof( SpriteBase::Vertex ) ) )
    {
        return false;
    }
    // 定数バッファを作成
    mCB = std::make_unique<ConstantBuffer>();
    if( !mCB->Create( sizeof( SpriteBase::Constant ) ) )
    {
        return false;
    }

    return true;
}

// 描画
void Sprite::Draw( const Matrix4& worldMat, Camera* camera )
{
    if( mTexture )
    {
        // 頂点バッファを更新
        UpdateVB();
        // 定数バッファを更新
        UpdateCB( worldMat, camera );

        // コマンドを積む
        auto cmdList = SpriteBase::GetInstance().mCmdList;
        cmdList->SetVertexBuffer( mVB.get() );
        cmdList->SetConstantBuffer( 0, mCB.get() );
        cmdList->SetGraphicsRootDescriptorTable( 1, mTexture->GetSRVHdl() );
        cmdList->DrawInstanced( kVertexCount );
    }
}

// テクスチャを設定
void Sprite::SetTexture( Texture* texture )
{
    mTexture = texture;
    if( mTexture )
    {
        mDrawRc.left = 0.0f;
        mDrawRc.top = 0.0f;
        mDrawRc.right = static_cast<float>( mTexture->GetWidth() );
        mDrawRc.bottom = static_cast<float>( mTexture->GetHeight() );
    }
}

// 頂点バッファを更新
void Sprite::UpdateVB()
{
    SpriteBase::Vertex v[4] = {};
    enum
    {
        LT,
        RT,
        LB,
        RB
    };

    // 座標
    auto w = static_cast<float>( mDrawRc.right - mDrawRc.left );
    auto h = static_cast<float>( mDrawRc.bottom - mDrawRc.top );
    auto l = ( 0.0f - mPivot.x ) * w;
    auto t = ( 0.0f - mPivot.y ) * h;
    auto r = ( 1.0f - mPivot.x ) * w;
    auto b = ( 1.0f - mPivot.y ) * h;
    if( mIsFlipX )
    {
        l = -l;
        r = -r;
    }
    if( mIsFlipY )
    {
        t = -t;
        b = -b;
    }
    v[LT].mPosition = Vector4( l, t, 0.0f, 1.0f );
    v[RT].mPosition = Vector4( r, t, 0.0f, 1.0f );
    v[LB].mPosition = Vector4( l, b, 0.0f, 1.0f );
    v[RB].mPosition = Vector4( r, b, 0.0f, 1.0f );

    // UV座標
    w = static_cast<float>( mTexture->GetWidth() );
    h = static_cast<float>( mTexture->GetHeight() );
    l = mDrawRc.left / w;
    t = mDrawRc.top / h;
    r = mDrawRc.right / w;
    b = mDrawRc.bottom / h;
    v[LT].mUV = Vector2( l, t );
    v[RT].mUV = Vector2( r, t );
    v[LB].mUV = Vector2( l, b );
    v[RB].mUV = Vector2( r, b );

    mVB->Update( v );
}

// 定数バッファを更新
void Sprite::UpdateCB( const Matrix4& worldMat, Camera* camera )
{
    SpriteBase::Constant c = {};

    // ワールドビュープロジェクション行列
    c.mWVP = worldMat * camera->GetView() * camera->GetProjection();

    // UVトランスフォーム行列
    c.mUVTransform =
        CreateScale( Vector3( mUVScale.x, mUVScale.y, 1.0f ) ) *
        CreateRotate( Quaternion( Vector3::kUnitZ, mUVRotate ) ) *
        CreateTranslate( Vector3( mUVTranslate.x, mUVTranslate.y, 0.0f ) );

    // 色
    c.mColor = mColor;

    mCB->Update( &c );
}

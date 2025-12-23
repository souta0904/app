#include "Mesh.h"

#include "core/CommandList.h"

// コンストラクタ
Mesh::Mesh()
    : mName( "Mesh" )
    , mFlags( MeshFlags::Required )
    , mVertices()
    , mVB( nullptr )
    , mIndices()
    , mIB( nullptr )
    , mMaterialIdx( 0 )
{
}

// 作成（頂点インデックスなし）
bool Mesh::Create( MeshFlags flags, const std::vector<Vertex>& vertices )
{
    mFlags = flags;
    mVertices = vertices;

    if( !CreateVB() ) return false;

    return true;
}

// 作成（頂点インデックスあり）
bool Mesh::Create( MeshFlags flags, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices )
{
    mFlags = flags;
    mVertices = vertices;
    mIndices = indices;

    if( !CreateVB() ) return false;

    if( !CreateIB() ) return false;

    return true;
}

// 描画
void Mesh::Draw( CommandList* cmdList )
{
    if( !cmdList || !mVB ) return;

    cmdList->SetVertexBuffer( mVB.get() );
    if( mIB )
    {
        // 頂点インデックスあり
        cmdList->SetIndexBuffer( mIB.get() );
        cmdList->DrawIndexedInstanced( static_cast<uint32_t>( mIndices.size() ) );
    }
    else
    {
        // 頂点インデックスなし
        cmdList->DrawInstanced( static_cast<uint32_t>( mVertices.size() ) );
    }
}

// 頂点バッファを作成
bool Mesh::CreateVB()
{
    mVB = std::make_unique<VertexBuffer>();
    if( !mVB->Create( static_cast<uint32_t>( mVertices.size() * sizeof( Vertex ) ), sizeof( Vertex ) ) )
    {
        return false;
    }
    mVB->Update( mVertices.data() );

    return true;
}

// インデックスバッファを作成
bool Mesh::CreateIB()
{
    mIB = std::make_unique<IndexBuffer>();
    if( !mIB->Create( static_cast<uint32_t>( mIndices.size() * sizeof( uint32_t ) ) ) )
    {
        return false;
    }
    mIB->Update( mIndices.data() );

    return true;
}

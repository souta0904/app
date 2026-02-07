#include "ModelData.h"

#include <format>

#include "core/ResourceManager.h"
#include "utils/StringHelper.h"

namespace
{

#ifdef _DEBUG
const bool kIsOutput = false;
#endif
const std::string kErrorTex = "assets/texture/error.png";

}  // namespace

// コンストラクタ
ModelData::ModelData()
    : mIsBuilt( false )
    , mAssimpScene( nullptr )
    , mNodeIdxToAssimpNode()
    , mNodeNameToNodeIdx()
    , mPath()
    , mNodes()
    , mRootNodeIdx( 0 )
    , mMeshCount( 0 )
    , mMeshes()
    , mMaterialCount( 0 )
    , mMaterials()
{
}

// モデルを構築
bool ModelData::Build( const std::string& path )
{
    mPath = path;
    // モデルを読み込む
    Assimp::Importer importer = {};
    mAssimpScene = importer.ReadFile( path.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs );
    if( !mAssimpScene )
    {
        return false;
    }

    // ノードを構築
    mRootNodeIdx = BuildNode( mAssimpScene->mRootNode, {} );
    // メッシュを構築
    BuildMesh( &mNodes[mRootNodeIdx] );
    // マテリアルを構築
    BuildMaterial();

#ifdef _DEBUG
    if( kIsOutput )
    {
        // ノードの階層をデバッグ表示
        OutputNodeInfo( &mNodes[mRootNodeIdx] );
    }
#endif

    // 解放
    importer.FreeScene();
    mAssimpScene = nullptr;
    mNodeIdxToAssimpNode.clear();
    mNodeNameToNodeIdx.clear();

    // ビルド完了！
    mIsBuilt = true;
    return true;
}

// ノードを構築
int32_t ModelData::BuildNode( aiNode* assimpNode, const std::optional<int32_t>& parent )
{
    ModelNode node = {};
    node.mName = assimpNode->mName.C_Str();  // ノード名

    // ノードのトランスフォーム
    aiVector3D scale;
    aiQuaternion rotate;
    aiVector3D translate;
    assimpNode->mTransformation.Decompose( scale, rotate, translate );
    node.mScale = Vector3( scale.x, scale.y, scale.z );
    node.mRotate = Quaternion( rotate.w, rotate.x, -rotate.y, -rotate.z );
    node.mTranslate = Vector3( -translate.x, translate.y, translate.z );
    node.mLocalMat = CreateAffine( node.mScale, node.mRotate, node.mTranslate );  // ローカル行列
    // モデル行列を計算
    if( parent )
    {
        node.mModelMat = node.mLocalMat * mNodes[*parent].mModelMat;
    }
    else
    {
        node.mModelMat = node.mLocalMat;
    }

    // 親子構造
    node.mIndex = static_cast<uint32_t>( mNodes.size() );
    node.mParent = parent;
    mNodes.push_back( node );
    // ヘルパーに追加
    mNodeIdxToAssimpNode.emplace( node.mIndex, assimpNode );
    mNodeNameToNodeIdx.emplace( node.mName, node.mIndex );
    // 再帰で子ノードを構築
    mNodes[node.mIndex].mChildren.resize( assimpNode->mNumChildren );
    for( uint32_t i = 0; i < assimpNode->mNumChildren; ++i )
    {
        mNodes[node.mIndex].mChildren[i] = BuildNode( assimpNode->mChildren[i], node.mIndex );
    }

    return node.mIndex;
}

// メッシュを構築
void ModelData::BuildMesh( ModelNode* node )
{
    auto assimpNode = mNodeIdxToAssimpNode[node->mIndex];
    for( uint32_t meshIdx = 0; meshIdx < assimpNode->mNumMeshes; ++meshIdx )
    {
        aiMesh* assimpMesh = mAssimpScene->mMeshes[assimpNode->mMeshes[meshIdx]];
        if( !assimpMesh->HasPositions() ||
            !assimpMesh->HasNormals() ||
            !assimpMesh->HasTextureCoords( 0 ) )
        {
            continue;
        }

        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
        auto meshFlags = MeshFlags::Required;
        // メッシュ名
        mesh->mName = assimpMesh->mName.C_Str();
        mesh->mAABB.Reset();

        // 頂点データ
        std::vector<Mesh::Vertex> vertices( assimpMesh->mNumVertices );
        for( uint32_t vertexIdx = 0; vertexIdx < assimpMesh->mNumVertices; ++vertexIdx )
        {
            auto& position = assimpMesh->mVertices[vertexIdx];
            auto& normal = assimpMesh->mNormals[vertexIdx];
            auto& uv = assimpMesh->mTextureCoords[0][vertexIdx];
            vertices[vertexIdx] = Mesh::Vertex{
                Vector4( -position.x, position.y, position.z, 1.0f ),
                Vector3( -normal.x, normal.y, normal.z ),
                Vector2( uv.x, uv.y ) };

            mesh->mAABB.Update( Vector3( -position.x, position.y, position.z ) );
        }

        // 頂点インデックスデータ
        std::vector<uint32_t> indices;
        for( uint32_t faceIdx = 0; faceIdx < assimpMesh->mNumFaces; ++faceIdx )
        {
            auto& face = assimpMesh->mFaces[faceIdx];
            if( face.mNumIndices > 4 )  // 五角形以上は無視
            {
                continue;
            }
            for( uint32_t i = 0; i < face.mNumIndices; ++i )
            {
                if( i == 3 )
                {
                    indices.emplace_back( face.mIndices[i - 1] );
                    indices.emplace_back( face.mIndices[i] );
                    indices.emplace_back( face.mIndices[i - 3] );
                }
                else
                {
                    indices.emplace_back( face.mIndices[i] );
                }
            }
        }

        // マテリアルのインデックス
        mesh->mMaterialIdx = assimpMesh->mMaterialIndex;

        // 作成
        mesh->Create( meshFlags, vertices, indices );

        // ノードとメッシュを紐付ける
        MeshData meshData = {};
        meshData.mNodeIdx = node->mIndex;
        meshData.mMesh = std::move( mesh );
        mMeshes.push_back( std::move( meshData ) );
        ++mMeshCount;
    }

    // 子メッシュを構築
    for( uint32_t i = 0; i < node->mChildren.size(); ++i )
    {
        BuildMesh( &mNodes[node->mChildren[i]] );
    }
}

// マテリアルを構築
void ModelData::BuildMaterial()
{
    ResourceManager& resMgr = ResourceManager::GetInstance();

    for( uint32_t i = 0; i < mAssimpScene->mNumMaterials; ++i )
    {
        aiMaterial* assimpMaterial = mAssimpScene->mMaterials[i];
        std::unique_ptr<Material> material = std::make_unique<Material>();

        // マテリアル名
        aiString name;
        assimpMaterial->Get( AI_MATKEY_NAME, name );
        material->mName = name.C_Str();

        // テクスチャ
        Texture* texture = nullptr;
        if( assimpMaterial->GetTextureCount( aiTextureType_DIFFUSE ) != 0 )
        {
            aiString texturePath;
            assimpMaterial->GetTexture( aiTextureType_DIFFUSE, 0, &texturePath );
            // NOTE: モデルファイルとテクスチャファイルは同じ階層に配置すること
            auto parentPath = StringHelper::GetParentPath( mPath );
            auto filename = StringHelper::GetFilename( texturePath.C_Str() );
            texture = resMgr.GetTexture( parentPath + "/" + filename );
            if( !texture )
            {
                // なければテクスチャフォルダから探す
                texture = resMgr.GetTexture( "assets/texture/" + filename );
                if( !texture )
                {
                    texture = resMgr.GetTexture( kErrorTex );
                }
            }
        }

        // 色
        aiColor4D color;
        assimpMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, color );
        material->mColor = Color( color.r, color.g, color.b, color.a );

        // 作成
        material->Create( texture );
        mMaterials.emplace_back( std::move( material ) );
        ++mMaterialCount;
    }
}

// 更新
void ModelData::Update()
{
    // すべてのマテリアルを更新
    for( auto& material : mMaterials )
    {
        material->Update();
    }
}

#ifdef _DEBUG

// ノードの階層をデバッグ表示
void ModelData::OutputNodeInfo( ModelNode* node, uint32_t depth )
{
    OutputDebugStringA( std::format( "{}{}\n", std::string( depth, '\t' ), node->mName ).c_str() );
    for( auto idx : node->mChildren )
    {
        OutputNodeInfo( &mNodes[idx], depth + 1 );
    }
}

#endif

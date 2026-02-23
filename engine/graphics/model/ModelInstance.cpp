#include "ModelInstance.h"

#include "MeshSorter.h"
#include "collision/Collision.h"
#include "core/CommandList.h"
#include "graphics/Camera.h"
#include "graphics/PrimitiveRenderer.h"
#include "graphics/Renderer.h"
#include "graphics/Texture.h"

// コンストラクタ
ModelInstance::ModelInstance()
    : mModelData( nullptr )
    , mNodes()
    , mTransMatCBs()
    , mMaterials()
{
}

// 作成
bool ModelInstance::Create( ModelData* modelData )
{
    // クリア
    mNodes.clear();
    mTransMatCBs.clear();
    mMaterials.clear();

    mModelData = modelData;
    if( mModelData )
    {
        // ノードリスト
        mNodes = mModelData->mNodes;

        // 変換行列
        mTransMatCBs.resize( mModelData->mMeshCount );
        for( uint32_t i = 0; i < mModelData->mMeshCount; ++i )
        {
            mTransMatCBs[i] = std::make_unique<ConstantBuffer>();
            if( !mTransMatCBs[i]->Create( sizeof( TransformationMatrix ) ) ) return false;
        }

        // マテリアルリスト
        mMaterials.resize( mModelData->mMaterialCount );
        for( uint32_t i = 0; i < mModelData->mMaterialCount; ++i )
        {
            mMaterials[i] = nullptr;
        }
    }

    return true;
}

// 更新
void ModelInstance::Update( float )
{
    if( !mModelData ) return;

    // 共通マテリアルの更新
    mModelData->Update();

    // TODO: アニメーション
}

// 描画
void ModelInstance::Draw( MeshSorter* sorter, const Matrix4& worldMat )
{
    if( !sorter || !mModelData ) return;

    // マテリアルを更新
    for( auto material : mMaterials )
    {
        if( !material ) continue;

        material->Update();
    }

    auto camera = sorter->GetCamera();
    if( !camera ) return;

    // AABB構築
    UpdateAABB( worldMat );

    // デバッグ描画
    auto& pr = PrimitiveRenderer::GetInstance();
    pr.DrawAABB( mWorldAABB, Color( 0.2f, 1.0f, 0.2f ) );

    auto& frustum = sorter->GetFrustumCamera()->GetFrustum();
    // フラスタムの外側はスキップ
    if( !Intersect( mWorldAABB, frustum ) ) return;

    // メッシュごと描画
    for( uint32_t i = 0; i < mModelData->mMeshCount; ++i )
    {
        auto& meshData = mModelData->mMeshes[i];
        auto mesh = meshData.mMesh.get();
        auto material = mMaterials[mesh->mMaterialIdx];
        if( !material )
        {
            material = mModelData->mMaterials[mesh->mMaterialIdx].get();
        }

        TransformationMatrix c = {};
        c.mWorld = mNodes[meshData.mNodeIdx].mModelMat * worldMat;
        auto wvMat = c.mWorld * camera->GetView();
        c.mWVP = wvMat * camera->GetProjection();
        c.mWorldInvTranspose = Transpose( InverseAffine( c.mWorld ) );
        mTransMatCBs[i]->Update( &c );

        // ソーターへ登録
        sorter->Add(
            MakePSOKey( mesh->mFlags, material->mFlags ),
            wvMat.m[3][2],  // Z値(カメラからの距離)
            mTransMatCBs[i].get(),
            mesh,
            material,
            mWorldAABB );
    }
}

// マテリアルを取得
Material* ModelInstance::GetMaterial( uint32_t idx )
{
    if( idx >= mMaterials.size() ) return nullptr;

    auto material = mMaterials[idx];
    if( !material )
    {
        if( idx >= mModelData->mMaterials.size() ) return nullptr;

        material = mModelData->mMaterials[idx].get();
    }

    return material;
}

// マテリアルを設定
void ModelInstance::SetMaterial( uint32_t idx, Material* material )
{
    if( idx >= mMaterials.size() ) return;

    mMaterials[idx] = material;
}

// AABBの更新
void ModelInstance::UpdateAABB( const Matrix4& worldMat )
{
    mWorldAABB.Reset();
    for( uint32_t i = 0; i < mModelData->mMeshCount; ++i )
    {
        auto local = mModelData->mMeshes[i].mMesh->mAABB;
        auto min = local.mMin;
        auto max = local.mMax;

        Vector3 v[8] = {};
        v[0] = min;
        v[1] = Vector3( max.x, min.y, min.z );
        v[2] = Vector3( max.x, min.y, max.z );
        v[3] = Vector3( min.x, min.y, max.z );
        v[4] = Vector3( min.x, max.y, min.z );
        v[5] = Vector3( max.x, max.y, min.z );
        v[6] = max;
        v[7] = Vector3( min.x, max.y, max.z );

        auto mat = mNodes[mModelData->mMeshes[i].mNodeIdx].mModelMat * worldMat;
        for( uint32_t j = 0; j < 8; ++j )
        {
            auto p = v[j] * mat;
            mWorldAABB.Update( p );
        }
    }
}

#include "ModelInstance.h"

#include "MeshSorter.h"
#include "core/CommandList.h"
#include "graphics/Camera.h"
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

    /*
    // 定数バッファを更新
    UpdateCB( worldMat, sorter->GetCamera() );
    */

    // メッシュごと描画
    auto view = camera->GetView();
    for( uint32_t i = 0; i < mModelData->mMeshCount; ++i )
    {
        auto mesh = mModelData->mMeshes[i].mMesh.get();
        auto material = mMaterials[mesh->mMaterialIdx];
        if( !material )
        {
            material = mModelData->mMaterials[mesh->mMaterialIdx].get();
        }

        TransformationMatrix c = {};
        c.mWorld = mNodes[mModelData->mMeshes[i].mNodeIdx].mModelMat * worldMat;
        auto wvMat = c.mWorld * view;
        c.mWVP = wvMat * camera->GetProjection();
        c.mWorldInvTranspose = Transpose( Inverse( c.mWorld ) );
        mTransMatCBs[i]->Update( &c );

        // TODO: フラスタムカリングの実装

        // ソーターへ登録
        sorter->Add(
            MakePSOKey( mesh->mFlags, material->mFlags ),
            wvMat.m[3][2],  // Z値(カメラからの距離)
            mTransMatCBs[i].get(),
            mesh,
            material );
    }
}

// マテリアルを設定
void ModelInstance::SetMaterial( uint32_t idx, Material* material )
{
    if( idx < 0 && idx >= mMaterials.size() ) return;

    mMaterials[idx] = material;
}

/*
// 定数バッファを更新
void ModelInstance::UpdateCB( const Matrix4& worldMat, Camera* camera )
{
    for( uint32_t i = 0; i < mModelData->mMeshCount; ++i )
    {
        TransformationMatrix c = {};
        c.mWorld = mNodes[mModelData->mMeshes[i].mNodeIdx].mModelMat * worldMat;
        c.mWVP = c.mWorld * camera->GetView() * camera->GetProjection();
        c.mWorldInvTranspose = Transpose( Inverse( c.mWorld ) );
        mTransformationMatrices[i]->Update( &c );
    }
}
*/

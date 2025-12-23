#include "ModelInstance.h"

#include "MeshSorter.h"
#include "core/CommandList.h"
#include "graphics/Camera.h"
#include "graphics/Texture.h"

// コンストラクタ
ModelInstance::ModelInstance()
    : mModelData( nullptr )
    , mNodes()
    , mTransformationMatrices()
    , mMaterials()
{
}

// 作成
bool ModelInstance::Create( ModelData* modelData )
{
    // クリア
    mNodes.clear();
    mTransformationMatrices.clear();
    mMaterials.clear();

    mModelData = modelData;
    if( mModelData )
    {
        // ノードリスト
        mNodes = mModelData->mNodes;

        // 変換行列
        mTransformationMatrices.resize( mModelData->mMeshCount );
        for( uint32_t i = 0; i < mModelData->mMeshCount; ++i )
        {
            mTransformationMatrices[i] = std::make_unique<ConstantBuffer>();
            if( !mTransformationMatrices[i]->Create( sizeof( TransformationMatrix ) ) ) return false;
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

    // 定数バッファを更新
    UpdateCB( worldMat, sorter->GetCamera() );

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

        auto wvMat = mNodes[mModelData->mMeshes[i].mNodeIdx].mModelMat * worldMat * view;
        auto distance = wvMat.m[3][2];

        // TODO: フラスタムカリングの実装

        // ソーターへ登録
        sorter->Add(
            MakePSOKey( mesh->mFlags, material->mFlags ),
            distance,
            mTransformationMatrices[i].get(),
            mesh,
            material );

        /*
        // コマンドを積む
        auto& modelBase = ModelBase::GetInstance();
        modelBase.SetGraphicsPSO( mesh->mFlags, material->mFlags );

        auto cmdList = modelBase.mCmdList;
        if( !cmdList ) return;

        cmdList->SetConstantBuffer( 0, mTransformationMatrices[i].get() );
        material->Bind( cmdList, 1, 2 );
        mesh->Draw( cmdList );
        */
    }
}

// マテリアルを設定
void ModelInstance::SetMaterial( uint32_t idx, Material* material )
{
    if( idx < 0 && idx >= mMaterials.size() ) return;

    mMaterials[idx] = material;
}

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

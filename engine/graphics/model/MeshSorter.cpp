#include "MeshSorter.h"

#include <algorithm>

#include "Material.h"
#include "Mesh.h"
#include "ModelBase.h"
#include "PSOKey.h"
#include "core/CommandList.h"
#include "core/DirectXBase.h"
#include "core/RootSignature.h"
#include "graphics/Camera.h"
#include "math/Vector3.h"

// コンストラクタ
MeshSorter::MeshSorter()
    : mCamera( nullptr )
    , mCameraCB( nullptr )
    , mSortItems()
{
}

// 初期化
bool MeshSorter::Init( Camera* camera )
{
    mCamera = camera;

    // 定数バッファを作成
    mCameraCB = std::make_unique<ConstantBuffer>();
    if( !mCameraCB->Create( sizeof( Vector3 ) ) )
    {
        return false;
    }

    mIndirectBuff = std::make_unique<RWStructuredBuffer>();
    if( !mIndirectBuff->Create( kMaxDrawCount, sizeof( IndirectCommand ) ) )
    {
        return false;
    }

    mIndirectUpload = std::make_unique<StructuredBuffer>();
    if( !mIndirectUpload->Create( kMaxDrawCount, sizeof( IndirectCommand ) ) )
    {
        return false;
    }

    return true;
}

// 描画アイテムの追加
void MeshSorter::Add( uint64_t psoKey, float distance, ConstantBuffer* transMatCB, Mesh* mesh, Material* material, const AABB3D& aabb )
{
    if( !transMatCB || !mesh || !material ) return;

    SortItem item = {};
    item.mPSOKey = psoKey;
    item.mDistance = distance;
    item.mQuantizedDist = QuantizeDist( distance );
    item.mTransMatCB = transMatCB;
    item.mMesh = mesh;
    item.mMaterial = material;
    item.mWorldAABB = aabb;
    mSortItems.emplace_back( item );
}

// ソート
void MeshSorter::Sort()
{
    std::sort(
        mSortItems.begin(),
        mSortItems.end(),
        []( const SortItem& a, const SortItem& b )
        {
            auto rqA = a.mMaterial->GetRenderQueue();
            auto rqB = b.mMaterial->GetRenderQueue();
            if( rqA != rqB ) return rqA < rqB;

            switch( rqA )
            {
                case RenderQueue::Opaque:
                    /*
                    // Front-to-Back
                    // 量子化距離 → PSOキー → 距離
                    if( a.mQuantizedDist != b.mQuantizedDist ) return a.mQuantizedDist < b.mQuantizedDist;

                    if( a.mPSOKey != b.mPSOKey ) return a.mPSOKey < b.mPSOKey;

                    return a.mDistance < b.mDistance;
                    */
                    return a.mPSOKey < b.mPSOKey;

                case RenderQueue::Transparent:
                    /*
                    // Back-to-Front
                    return a.mDistance > b.mDistance;
                    */
                    return a.mPSOKey < b.mPSOKey;

                default:
                    return false;
            }
        } );
}

// z-prepass描画
void MeshSorter::RenderZPrepass( CommandList* cmdList )
{
    if( !cmdList || mSortItems.empty() )
    {
        return;
    }

    for( auto& item : mSortItems )
    {
        if( !item.mIsVisible ) continue;

        if( item.mTransMatCB )
        {
            cmdList->SetGraphicsConstantBuffer( 0, item.mTransMatCB );
        }

        if( item.mMesh )
        {
            item.mMesh->Draw( cmdList );
        }
    }
}

// 描画
void MeshSorter::Render( CommandList* cmdList )
{
    if( !cmdList || mSortItems.empty() )
    {
        return;
    }

    UpdateIndirectBuff( cmdList );

    // カメラ
    mCameraCB->Update( &mCamera->mPosition );
    cmdList->SetGraphicsConstantBuffer( 2, mCameraCB.get() );

    auto currPSOKey = UINT64_MAX;
    if( mUseIndirectDraw )
    {
        currPSOKey = mSortItems[0].mPSOKey;
    }
    auto& modelBase = ModelBase::GetInstance();
    auto commandSignature = modelBase.GetCommandSignature();

    auto offsetIdx = 0u;
    for( uint32_t i = 0; i < mSortItems.size(); ++i )
    {
        if( mUseIndirectDraw )
        {
            if( mSortItems[i].mPSOKey != currPSOKey )
            {
                modelBase.SetGraphicsPSO( currPSOKey );

                cmdList->ExecuteIndirect(
                    commandSignature.Get(),
                    i - offsetIdx,
                    mIndirectBuff->GetResource().Get(),
                    sizeof( IndirectCommand ) * offsetIdx );

                currPSOKey = mSortItems[i].mPSOKey;
                offsetIdx = i;
            }
        }
        else
        {
            auto& item = mSortItems[i];

            if( !item.mIsVisible ) continue;

            // パイプラインステート
            if( currPSOKey != item.mPSOKey )
            {
                modelBase.SetGraphicsPSO( item.mPSOKey );
                currPSOKey = item.mPSOKey;
            }

            // 変換行列
            if( item.mTransMatCB )
            {
                cmdList->SetGraphicsConstantBuffer( 0, item.mTransMatCB );
            }

            // マテリアル
            if( item.mMaterial )
            {
                item.mMaterial->Bind( cmdList, 1, 3 );
            }

            if( item.mMesh )
            {
                item.mMesh->Draw( cmdList );
            }
        }
    }

    if( mUseIndirectDraw )
    {
        modelBase.SetGraphicsPSO( currPSOKey );

        cmdList->ExecuteIndirect(
            commandSignature.Get(),
            static_cast<uint32_t>( mSortItems.size() ) - offsetIdx,
            mIndirectBuff->GetResource().Get(),
            sizeof( IndirectCommand ) * offsetIdx );
    }

    mSortItems.clear();
}

// 距離を量子化
uint32_t MeshSorter::QuantizeDist( float distance )
{
    return static_cast<uint32_t>( distance / 1.0f );  // 1m単位
}

void MeshSorter::UpdateIndirectBuff( CommandList* cmdList )
{
    IndirectCommand data[kMaxDrawCount];
    for( uint32_t i = 0; i < mSortItems.size(); ++i )
    {
        auto& item = mSortItems[i];

        data[i].mVBV = item.mMesh->GetVBV();
        data[i].mIBV = item.mMesh->GetIBV();
        data[i].mTransMatCBV = item.mTransMatCB->GetGPUVirtualAddress();
        data[i].mMaterialCBV = item.mMaterial->GetConstantBuffer()->GetGPUVirtualAddress();
        data[i].mDrawArg.IndexCountPerInstance = item.mMesh->GetIndicesCount();
        data[i].mDrawArg.InstanceCount = 1;
        data[i].mDrawArg.StartIndexLocation = 0;
        data[i].mDrawArg.BaseVertexLocation = 0;
        data[i].mDrawArg.StartInstanceLocation = 0;
    }
    mIndirectUpload->Update( data );

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = mIndirectBuff->GetResource().Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    cmdList->ResourceBarrier( barrier );

    cmdList->CopyResource( mIndirectBuff->GetResource().Get(), mIndirectUpload->GetResource().Get() );

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    cmdList->ResourceBarrier( barrier );
}

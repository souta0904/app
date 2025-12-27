#include "MeshSorter.h"

#include <algorithm>

#include "Material.h"
#include "Mesh.h"
#include "ModelBase.h"
#include "PSOKey.h"
#include "core/CommandList.h"
#include "math/Vector3.h"

// コンストラクタ
MeshSorter::MeshSorter()
    : mCamera( nullptr )
    , mCameraCB( nullptr )
    , mSortItems()
{
}

// 初期化
bool MeshSorter::Initialize( Camera* camera )
{
    mCamera = camera;

    // 定数バッファを作成
    mCameraCB = std::make_unique<ConstantBuffer>();
    if( !mCameraCB->Create( sizeof( Vector3 ) ) )
    {
        return false;
    }

    return true;
}

// 描画アイテムの追加
void MeshSorter::Add( uint64_t psoKey, float distance, ConstantBuffer* transMatCB, Mesh* mesh, Material* material )
{
    if( !transMatCB || !mesh || !material ) return;

    SortItem item = {};
    item.mPSOKey = psoKey;
    item.mDistance = distance;
    item.mQuantizedDist = QuantizeDist( distance );
    item.mTransMatCB = transMatCB;
    item.mMesh = mesh;
    item.mMaterial = material;
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
                    // Front-to-Back
                    // 量子化距離 → PSOキー → 距離
                    if( a.mQuantizedDist != b.mQuantizedDist ) return a.mQuantizedDist < b.mQuantizedDist;

                    if( a.mPSOKey != b.mPSOKey ) return a.mPSOKey < b.mPSOKey;

                    return a.mDistance < b.mDistance;

                case RenderQueue::Transparent:
                    // Back-to-Front
                    return a.mDistance > b.mDistance;

                default:
                    return false;
            }
        } );
}

// 描画
void MeshSorter::Render( CommandList* cmdList )
{
    if( !cmdList || mSortItems.empty() )
    {
        return;
    }

    auto currPSOKey = UINT64_MAX;
    auto& modelBase = ModelBase::GetInstance();
    for( auto& item : mSortItems )
    {
        // パイプラインステート
        if( currPSOKey != item.mPSOKey )
        {
            modelBase.SetGraphicsPSO( item.mPSOKey );
            currPSOKey = item.mPSOKey;
        }

        // 変換行列
        if( item.mTransMatCB )
        {
            cmdList->SetConstantBuffer( 0, item.mTransMatCB );
        }

        // マテリアル
        if( item.mMaterial )
        {
            item.mMaterial->Bind( cmdList, 1, 3 );
        }

        // カメラ
        cmdList->SetConstantBuffer( 2, mCameraCB.get() );

        if( item.mMesh )
        {
            item.mMesh->Draw( cmdList );
        }
    }

    mSortItems.clear();
}

// 距離を量子化
uint32_t MeshSorter::QuantizeDist( float distance )
{
    return static_cast<uint32_t>( distance / 1.0f );  // 1m単位
}

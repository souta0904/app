#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

class CommandList;

/// <summary>
/// 頂点バッファ
/// </summary>
class VertexBuffer
{
   private:
    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW mView;
    // データへのポインタ
    void* mData;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    VertexBuffer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~VertexBuffer() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="size">頂点バッファのサイズ</param>
    /// <param name="strideSize">1頂点のサイズ</param>
    /// <returns>成否</returns>
    bool Create( uint32_t size, uint32_t strideSize );

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="data">データ</param>
    void Update( void* data );

    /// <summary>頂点バッファビューを取得</summary>
    const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return mView; }
};

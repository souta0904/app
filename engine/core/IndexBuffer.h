#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

/// <summary>
/// インデックスバッファ
/// </summary>
class IndexBuffer
{
   private:
    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    // インデックスバッファビュー
    D3D12_INDEX_BUFFER_VIEW mView;
    // データへのポインタ
    void* mData;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    IndexBuffer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~IndexBuffer() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="size">サイズ</param>
    /// <returns>成否</returns>
    bool Create( uint32_t size );

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="data">データ</param>
    void Update( void* data );

    /// <summary>インデックスバッファビューを取得</summary>
    const D3D12_INDEX_BUFFER_VIEW& GetView() const { return mView; }
};

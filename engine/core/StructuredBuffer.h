#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

/// <summary>
/// 構造化バッファ
/// </summary>
class StructuredBuffer
{
    friend class DirectXBase;

   private:
    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    // 要素数
    uint32_t mCount;
    // 1要素のサイズ
    uint32_t mStrideSize;
    // サイズ
    uint32_t mSize;
    // データへのポインタ
    void* mData;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    StructuredBuffer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~StructuredBuffer();

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="count">要素数</param>
    /// <param name="strideSize">1要素のサイズ</param>
    /// <returns>成否</returns>
    bool Create( uint32_t count, uint32_t strideSize );

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="data">データ</param>
    void Update( const void* data );

    /// <summary>リソースを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return mResource; }
};

#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

/// <summary>
/// 読み取り/書き込みバッファ
/// </summary>
class RWStructuredBuffer
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
    RWStructuredBuffer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~RWStructuredBuffer() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="count">要素数</param>
    /// <param name="strideSize">1要素のサイズ</param>
    /// <returns>成否</returns>
    bool Create( uint32_t count, uint32_t strideSize );

    /// <summary>リソースを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return mResource; }

    /// <summary>サイズを取得</summary>
    uint32_t GetSize() const { return mSize; }
};

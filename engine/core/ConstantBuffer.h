#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

/// <summary>
/// 定数バッファ
/// </summary>
class ConstantBuffer
{
   private:
    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    // サイズ
    uint32_t mSize;
    // データへのポインタ
    void* mData;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ConstantBuffer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ConstantBuffer() = default;

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

    /// <summary>GPU仮想アドレスを取得</summary>
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return mResource->GetGPUVirtualAddress(); }
};

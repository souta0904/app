#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

class CommandList;

/// <summary>
/// 読み戻しバッファ
/// </summary>
class ReadbackBuffer
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
    ReadbackBuffer();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ReadbackBuffer() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="srcBuff">元リソース</param>
    /// <returns>成否</returns>
    bool Create( Microsoft::WRL::ComPtr<ID3D12Resource> srcBuff );

    /// <summary>
    /// マップ
    /// </summary>
    void* Map();

    /// <summary>
    /// アンマップ
    /// </summary>
    void Unmap();

    /// <summary>
    /// コピー
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    /// <param name="srcBuff">元リソース</param>
    void CopyFrom( CommandList* cmdList, Microsoft::WRL::ComPtr<ID3D12Resource> srcBuff );

    /// <summary>リソースを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return mResource; }
};

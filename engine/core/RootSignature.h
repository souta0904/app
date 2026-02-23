#pragma once
#include <wrl.h>

#include <memory>

#include "RootParameter.h"

class CommandList;

/// <summary>
/// ルートシグネチャ
/// </summary>
class RootSignature
{
   private:
    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    // ルートパラメータの数
    uint32_t mNumParameters;
    // ルートパラメータ
    std::unique_ptr<RootParameter[]> mParameters;
    // サンプラーの数
    uint32_t mNumSamplers;
    // サンプラー
    std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    RootSignature();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~RootSignature() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="numParameters">ルートパラメータの数</param>
    /// <param name="numSamplers">サンプラーの数</param>
    void Init( uint32_t numParameters, uint32_t numSamplers );

    /// <summary>
    /// ルートパラメータを取得
    /// </summary>
    /// <param name="idx">インデックス</param>
    /// <returns>ルートパラメータ</returns>
    RootParameter& GetParameter( uint32_t idx );

    /// <summary>
    /// サンプラーを取得
    /// </summary>
    /// <param name="idx">インデックス</param>
    /// <returns>サンプラー</returns>
    D3D12_STATIC_SAMPLER_DESC& GetSampler( uint32_t idx );

    /// <summary>
    /// 作成
    /// </summary>
    /// <returns>成否</returns>
    bool Create( D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT );

    /// <summary>ルートシグネチャを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return mRootSignature; }
};

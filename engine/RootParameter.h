#pragma once
#include <d3d12.h>

#include <cstdint>
#include <memory>

/// <summary>
/// ルートパラメータ
/// </summary>
class RootParameter
{
   private:
    // ルートパラメータ
    D3D12_ROOT_PARAMETER mRootParameter;
    // デスクリプタレンジ
    std::unique_ptr<D3D12_DESCRIPTOR_RANGE[]> mDescriptorRanges;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    RootParameter();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~RootParameter() = default;

    /// <summary>
    /// 定数バッファビューとして初期化
    /// </summary>
    /// <param name="shaderRegister">レジスタ番号</param>
    /// <param name="shaderVisibility">使用されるシェーダーステージ</param>
    void InitAsCBV( uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL );

    /// <summary>
    /// デスクリプタテーブルとして初期化
    /// </summary>
    /// <param name="numDescriptorRanges">デスクリプタレンジの数</param>
    /// <param name="shaderVisibility">使用されるシェーダーステージ</param>
    void InitAsDescriptorTable( uint32_t numDescriptorRanges, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL );

    /// <summary>
    /// デスクリプタレンジを設定
    /// </summary>
    /// <param name="idx">インデックス</param>
    /// <param name="rangeType">デスクリプタレンジの種類</param>
    /// <param name="numDescriptors">デスクリプタレンジの数</param>
    /// <param name="baseShaderRegister">開始レジスタ番号</param>
    void SetDescriptorRange( uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister );

    /// <summary>ルートパラメータを取得</summary>
    const D3D12_ROOT_PARAMETER& GetRootParameter() const { return mRootParameter; }
};

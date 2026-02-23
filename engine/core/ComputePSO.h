#pragma once
#include <wrl.h>

#include "PSOInit.h"

class CommandList;

/// <summary>
/// コンピュートパイプラインステートオブジェクト
/// </summary>
class ComputePSO
{
   private:
    // パイプラインステート
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ComputePSO();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ComputePSO() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="init">PSO初期化構造体</param>
    /// <returns>成否</returns>
    bool Create( const ComputePSOInit& init );

    /// <summary>パイプラインステートを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const { return mPipelineState; }
};

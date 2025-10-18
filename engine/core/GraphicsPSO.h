#pragma once
#include <wrl.h>

#include <future>
#include <vector>

#include "PSOInit.h"

class CommandList;

/// <summary>
/// グラフィックスパイプラインステートオブジェクト
/// </summary>
class GraphicsPSO
{
   public:
    /// <summary>
    /// 状態
    /// </summary>
    enum class State
    {
        Unload,
        Load,
        Ready,
        Error,
    };

   private:
    // 状態
    std::atomic<State> mState;
    // タスク
    std::future<void> mTask;

    // パイプラインステート設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc;
    // 頂点レイアウト
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
    // パイプラインステート
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    GraphicsPSO();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GraphicsPSO() = default;

    /// <summary>
    /// コピーコンストラクタ
    /// </summary>
    GraphicsPSO( const GraphicsPSO& pso );

    /// <summary>
    /// 代入演算子
    /// </summary>
    GraphicsPSO& operator=( const GraphicsPSO& pso );

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="init">PSO初期化構造体</param>
    void Create( const PSOInit& init );

    /// <summary>
    /// バインド
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Bind( CommandList* cmdList );

    /// <summary>使用可能か</summary>
    bool IsReady() { return mState == State::Ready; }

    /// <summary>パイプラインステートを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const { return mPipelineState; }
};

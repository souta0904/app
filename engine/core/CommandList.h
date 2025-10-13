#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <vector>

#include "DescriptorHeap.h"

class GraphicsPSO;
class RootSignature;

/// <summary>
/// コマンドリスト
/// </summary>
class CommandList
{
   private:
    // コマンドアロケーター
    std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCmdAllocators;
    // コマンドリスト
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    CommandList();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CommandList() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="count">コマンドアロケーターの数</param>
    /// <returns>成否</returns>
    bool Create( uint32_t count = 1 );

    /// <summary>
    /// クローズ
    /// </summary>
    void Close();

    /// <summary>
    /// リセット
    /// </summary>
    /// <param name="idx">インデックス</param>
    void Reset( uint32_t idx = 0 );

#pragma region ID3D12GraphicsCommandListラッパー

    /// <summary>
    /// デスクリプタヒープをセット
    /// </summary>
    /// <param name="descriptorHeap">デスクリプタヒープ</param>
    void SetDescriptorHeap( DescriptorHeap* descriptorHeap );

    /// <summary>
    /// リソースバリアをセット
    /// </summary>
    /// <param name="barrier">リソースバリア</param>
    void ResourceBarrier( const D3D12_RESOURCE_BARRIER& barrier );

    /// <summary>
    /// レンダーターゲットをセット
    /// </summary>
    /// <param name="hRTV">RTVのCPUデスクリプタハンドル</param>
    /// <param name="hDSV">DSVのCPUデスクリプタハンドル</param>
    void SetRenderTarget( DescriptorHandle* hRTV, DescriptorHandle* hDSV );

    /// <summary>
    /// レンダーターゲットをクリア
    /// </summary>
    /// <param name="hRTV">RTVのCPUデスクリプタハンドル</param>
    /// <param name="clearColor">クリアカラー</param>
    void ClearRenderTargetView( DescriptorHandle* hRTV, const float clearColor[4] );

    /// <summary>
    /// 深度バッファをクリア
    /// </summary>
    /// <param name="hDSV">DSVのCPUデスクリプタハンドル</param>
    void ClearDepthStencilView( DescriptorHandle* hDSV );

    /// <summary>
    /// デスクリプタテーブルをルートシグネチャへセット
    /// </summary>
    /// <param name="rootParamIdx">ルートパラメータのインデックス</param>
    /// <param name="hSRV">SRVのCPUデスクリプタハンドル</param>
    void SetGraphicsRootDescriptorTable( uint32_t rootParamIdx, DescriptorHandle* hSRV );

    /// <summary>
    /// 頂点バッファをセット
    /// </summary>
    /// <param name="view">頂点バッファビュー</param>
    void SetVertexBuffer( const D3D12_VERTEX_BUFFER_VIEW& vbv );

    /// <summary>
    /// ルートシグネチャをセット
    /// </summary>
    /// <param name="rootSignature">ルートシグネチャ</param>
    void SetGraphicsRootSignature( RootSignature* rootSignature );

    /// <summary>
    /// パイプラインステートをセット
    /// </summary>
    /// <param name="pso">パイプラインステート</param>
    void SetPipelineState( GraphicsPSO* pso );

    /// <summary>
    /// ビューポートをセット
    /// </summary>
    /// <param name="topLeftX">左上X座標</param>
    /// <param name="topLeftY">左上Y座標</param>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    /// <param name="minDepth">最小深度値</param>
    /// <param name="maxDepth">最大深度値</param>
    void SetViewport( float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f );

    /// <summary>
    /// シザー矩形をセット
    /// </summary>
    /// <param name="left">左</param>
    /// <param name="top">上</param>
    /// <param name="right">右</param>
    /// <param name="bottom">下</param>
    void SetScissorRect( float left, float top, float right, float bottom );

    /// <summary>
    /// プリミティブ型をセット
    /// </summary>
    /// <param name="primitiveTopology">プリミティブ型</param>
    void SetPrimitiveTopology( D3D12_PRIMITIVE_TOPOLOGY primitiveTopology );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="vertexCount">頂点数</param>
    void DrawInstanced(uint32_t vertexCount);

#pragma endregion

    /// <summary>コマンドリストを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCmdList() const { return mCmdList; }
};

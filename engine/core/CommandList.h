#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <vector>

#include "DescriptorHeap.h"

class ConstantBuffer;
class GraphicsPSO;
class IndexBuffer;
class RootSignature;
class VertexBuffer;

/// <summary>
/// コマンドリスト
/// </summary>
class CommandList
{
   public:
    /// <summary>
    /// コマンドリスト種別
    /// </summary>
    enum class Type
    {
        Direct,
        Copy,
    };

   private:
    // コマンドアロケーター
    std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCmdAllocators;
    // コマンドリスト
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;

   public:
    /// <summary>
    /// DirectX12のコマンドリスト種別を取得
    /// </summary>
    /// <param name="type">コマンドリスト種別</param>
    /// <returns>DirectX12のコマンドリスト種別</returns>
    static D3D12_COMMAND_LIST_TYPE GetCommandListType( Type type );

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
    /// <param name="type">コマンドリスト種別</param>
    /// <param name="count">コマンドアロケーターの数</param>
    /// <returns>成否</returns>
    bool Create( Type type, uint32_t count = 1 );

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
    /// 深度バッファをクリア
    /// </summary>
    /// <param name="hDSV">DSVのCPUデスクリプタハンドル</param>
    void ClearDepthStencilView( DescriptorHandle* hDSV );

    /// <summary>
    /// レンダーターゲットをクリア
    /// </summary>
    /// <param name="hRTV">RTVのCPUデスクリプタハンドル</param>
    /// <param name="clearColor">クリアカラー</param>
    void ClearRenderTargetView( DescriptorHandle* hRTV, const float clearColor[4] );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="vertexCount">頂点数</param>
    void DrawInstanced( uint32_t vertexCount );

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="indexCount">インデックス数</param>
    void DrawIndexedInstanced( uint32_t indexCount );

    /// <summary>
    /// リソースバリアをセット
    /// </summary>
    /// <param name="barrier">リソースバリア</param>
    void ResourceBarrier( const D3D12_RESOURCE_BARRIER& barrier );

    /// <summary>
    /// 定数バッファをセット
    /// </summary>
    /// <param name="rootParamIdx">ルートパラメータのインデックス</param>
    /// <param name="constantBuffer">定数バッファ</param>
    void SetConstantBuffer( uint32_t rootParamIdx, ConstantBuffer* constantBuffer );

    /// <summary>
    /// デスクリプタヒープをセット
    /// </summary>
    /// <param name="descriptorHeap">デスクリプタヒープ</param>
    void SetDescriptorHeap( DescriptorHeap* descriptorHeap );

    /// <summary>
    /// デスクリプタテーブルをルートシグネチャへセット
    /// </summary>
    /// <param name="rootParamIdx">ルートパラメータのインデックス</param>
    /// <param name="hSRV">SRVのCPUデスクリプタハンドル</param>
    void SetGraphicsRootDescriptorTable( uint32_t rootParamIdx, DescriptorHandle* hSRV );

    /// <summary>
    /// ルートシグネチャをセット
    /// </summary>
    /// <param name="rootSignature">ルートシグネチャ</param>
    void SetGraphicsRootSignature( RootSignature* rootSignature );

    /// <summary>
    /// インデックスバッファをセット
    /// </summary>
    /// <param name="indexBuffer"></param>
    void SetIndexBuffer( IndexBuffer* indexBuffer );

    /// <summary>
    /// パイプラインステートをセット
    /// </summary>
    /// <param name="pso">パイプラインステート</param>
    void SetPipelineState( GraphicsPSO* pso );

    /// <summary>
    /// プリミティブ型をセット
    /// </summary>
    /// <param name="primitiveTopology">プリミティブ型</param>
    void SetPrimitiveTopology( D3D12_PRIMITIVE_TOPOLOGY primitiveTopology );

    /// <summary>
    /// レンダーターゲットをセット
    /// </summary>
    /// <param name="hRTV">RTVのCPUデスクリプタハンドル</param>
    /// <param name="hDSV">DSVのCPUデスクリプタハンドル</param>
    void SetRenderTarget( DescriptorHandle* hRTV, DescriptorHandle* hDSV );

    /// <summary>
    /// シザー矩形をセット
    /// </summary>
    /// <param name="left">左</param>
    /// <param name="top">上</param>
    /// <param name="right">右</param>
    /// <param name="bottom">下</param>
    void SetScissorRect( float left, float top, float right, float bottom );

    /// <summary>
    /// 頂点バッファをセット
    /// </summary>
    /// <param name="vertexBuffer">頂点バッファ</param>
    void SetVertexBuffer( VertexBuffer* vertexBuffer );

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

#pragma endregion

    /// <summary>コマンドリストを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCmdList() const { return mCmdList; }
};

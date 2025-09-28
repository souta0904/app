#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <memory>
#include <string>

#include "CommandList.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"

class Window;

/// <summary>
/// DirectX基盤
/// </summary>
class DirectXBase
{
   public:
    // バックバッファの数
    static const uint32_t kBackBuffCount = 2;

   private:
    // DXGIファクトリー
    Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory;
    // デバイス
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    // コマンドキュー
    std::unique_ptr<CommandQueue> mCmdQueue;
    // スワップチェイン
    Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
    // コマンドリスト
    std::unique_ptr<CommandList> mCmdList;
    // デスクリプタヒープ
    std::unique_ptr<DescriptorHeap> mRTVHeap;
    std::unique_ptr<DescriptorHeap> mDSVHeap;
    std::unique_ptr<DescriptorHeap> mSRVHeap;
    // バックバッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffs[kBackBuffCount];
    DescriptorHandle* mRTVHdls[kBackBuffCount];
    // 深度バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
    DescriptorHandle* mDSVHdl;
    // 現在のバックバッファインデックス
    uint32_t mBackBuffIdx;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static DirectXBase& GetInstance()
    {
        static DirectXBase instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    DirectXBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~DirectXBase() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    DirectXBase( const DirectXBase& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    DirectXBase& operator=( const DirectXBase& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    DirectXBase( DirectXBase&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    DirectXBase& operator=( DirectXBase&& ) = delete;

   public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// 描画開始
    /// </summary>
    void BeginDraw();

    /// <summary>
    /// 描画終了
    /// </summary>
    void EndDraw();

    /// <summary>デバイスを取得</summary>
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return mDevice.Get(); }

    /// <summary>コマンドリストを取得</summary>
    CommandList* GetCmdList() const { return mCmdList.get(); }

    /// <summary>RTV用デスクリプタヒープを取得</summary>
    DescriptorHeap* GetRTVHeap() const { return mRTVHeap.get(); }

    /// <summary>DSV用デスクリプタヒープを取得</summary>
    DescriptorHeap* GetDSVHeap() const { return mDSVHeap.get(); }

    /// <summary>SRV用デスクリプタヒープを取得</summary>
    DescriptorHeap* GetSRVHeap() const { return mSRVHeap.get(); }

   private:
    /// <summary>
    /// デバイスを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateDevice();

    /// <summary>
    /// コマンドキューを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateCmdQueue();

    /// <summary>
    /// スワップチェインを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateSwapChain();

    /// <summary>
    /// コマンドリストを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateCmdList();

    /// <summary>
    /// デスクリプタヒープを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateDescriptorHeaps();

    /// <summary>
    /// レンダーターゲットビューを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateRTV();

    /// <summary>
    /// 深度ステンシルビューを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateDSV();
};

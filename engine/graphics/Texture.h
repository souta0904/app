#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <string>

#include "DirectXTex/DirectXTex.h"
#include "core/DescriptorHeap.h"

class CommandList;

/// <summary>
/// テクスチャ
/// </summary>
class Texture
{
   private:
    // テクスチャのパス
    std::string mPath;
    // テクスチャの幅
    uint32_t mWidth;
    // テクスチャの高さ
    uint32_t mHeight;

    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    // デスクリプタハンドル
    DescriptorHandle* mSRVHdl;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Texture();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Texture();

    /// <summary>
    /// テクスチャを作成
    /// </summary>
    /// <param name="path">テクスチャのパス</param>
    /// <returns>成否</returns>
    bool Create( const std::string& path );

    /// <summary>テクスチャのパスを取得</summary>
    const std::string& GetPath() const { return mPath; }

    /// <summary>テクスチャの幅を取得</summary>
    uint32_t GetWidth() const { return mWidth; }

    /// <summary>テクスチャの高さを取得</summary>
    uint32_t GetHeight() const { return mHeight; }

    /// <summary>デスクリプタハンドルを取得</summary>
    DescriptorHandle* GetSRVHdl() const { return mSRVHdl; }

   private:
    /// <summary>
    /// テクスチャを読み込む
    /// </summary>
    /// <param name="path">テクスチャのパス</param>
    /// <param name="image">テクスチャデータ</param>
    /// <param name="mipChain">ミップマップ付きデータ</param>
    /// <returns>成否</returns>
    bool Load( const std::string& path, DirectX::ScratchImage& image, DirectX::ScratchImage& mipChain );

    /// <summary>
    /// リソースを作成
    /// </summary>
    /// <param name="mipChain">ミップマップ付きデータ</param>
    /// <returns>成否</returns>
    bool CreateResource( const DirectX::ScratchImage& mipChain );

    /// <summary>
    /// データをアップロード
    /// </summary>
    /// <param name="mipChain">ミップマップ付きデータ</param>
    /// <returns>成否</returns>
    bool Upload( const DirectX::ScratchImage& mipChain );

    /// <summary>
    /// シェーダーリソースビューを作成
    /// </summary>
    /// <returns>成否</returns>
    bool CreateSRV();
};

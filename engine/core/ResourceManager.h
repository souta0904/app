#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "graphics/ShaderObject.h"
#include "graphics/Texture.h"

/// <summary>
/// リソース管理
/// </summary>
class ResourceManager
{
   private:
    // DXCユーティリティ
    Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
    // DXCコンパイラ
    Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
    // インクルードハンドラ
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;

    // シェーダーオブジェクト
    std::unordered_map<std::string, std::unique_ptr<ShaderObject>> mShaders;
    // テクスチャ
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static ResourceManager& GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

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
    /// シェーダーオブジェクトを取得
    /// </summary>
    /// <param name="path">シェーダーのパス</param>
    /// <param name="profile">プロファイル</param>
    /// <returns>シェーダーオブジェクト</returns>
    ShaderObject* GetShader( const std::string& path, const std::string& profile );

    /// <summary>
    /// テクスチャを取得
    /// </summary>
    /// <param name="path">テクスチャのパス</param>
    /// <returns>テクスチャ</returns>
    Texture* GetTexture( const std::string& path );

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ResourceManager();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ResourceManager() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    ResourceManager( const ResourceManager& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    ResourceManager& operator=( const ResourceManager& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    ResourceManager( ResourceManager&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    ResourceManager& operator=( ResourceManager&& ) = delete;

    /// <summary>
    /// DirectXシェーダーコンパイラを初期化
    /// </summary>
    /// <returns>成否</returns>
    bool InitDXC();
};

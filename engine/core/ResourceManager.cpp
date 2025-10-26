#include "ResourceManager.h"

#pragma comment( lib, "dxcompiler.lib" )

// コンストラクタ
ResourceManager::ResourceManager()
    : mUtils( nullptr )
    , mCompiler( nullptr )
    , mIncludeHandler( nullptr )
    , mShaders()
{
}

// 初期化
bool ResourceManager::Init()
{
    if( !InitDXC() )
    {
        return false;
    }

    return true;
}

// 終了処理
void ResourceManager::Term()
{
}

// シェーダーオブジェクトを取得
ShaderObject* ResourceManager::GetShader( const std::string& path, const std::string& profile )
{
    auto it = mShaders.find( path );
    if( it != mShaders.end() )
    {
        return it->second.get();
    }
    else
    {
        auto shader = std::make_unique<ShaderObject>();
        if( shader->Compile( path, profile, mUtils, mCompiler, mIncludeHandler ) )
        {
            auto* ptr = shader.get();
            mShaders.emplace( path, std::move( shader ) );
            return ptr;
        }
    }
    return nullptr;
}

// テクスチャを取得
Texture* ResourceManager::GetTexture( const std::string& path )
{
    auto it = mTextures.find( path );
    if( it != mTextures.end() )
    {
        return it->second.get();
    }
    else
    {
        auto texture = std::make_unique<Texture>();
        if( texture->Create( path ) )
        {
            auto* ptr = texture.get();
            mTextures.emplace( path, std::move( texture ) );
            return ptr;
        }
    }
    return nullptr;
}

// DirectXシェーダーコンパイラを初期化
bool ResourceManager::InitDXC()
{
    auto hr = DxcCreateInstance( CLSID_DxcUtils, IID_PPV_ARGS( &mUtils ) );
    if( FAILED( hr ) ) return false;

    hr = DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( &mCompiler ) );
    if( FAILED( hr ) ) return false;

    hr = mUtils->CreateDefaultIncludeHandler( &mIncludeHandler );
    if( FAILED( hr ) ) return false;

    return true;
}

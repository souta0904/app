#include "ShaderObject.h"

#include <format>

#include "utils/Logger.h"
#include "utils/StringHelper.h"

// コンストラクタ
ShaderObject::ShaderObject()
    : mPath()
    , mProfile()
    , mBlob( nullptr )
{
}

// コンパイル
bool ShaderObject::Compile(
    const std::string& path,
    const std::string& profile,
    Microsoft::WRL::ComPtr<IDxcUtils> utils,
    Microsoft::WRL::ComPtr<IDxcCompiler3> compiler,
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler )
{
    if( !utils || !compiler || !includeHandler )
    {
        LOG_INFO( std::format( "Failed to compile Shader: {}", mPath ) );
        return false;
    }

    mPath = path;
    mProfile = profile;

    // シェーダーファイルを読み込む
    auto wpath = StringHelper::Convert( mPath );
    Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob = nullptr;
    auto hr = utils->LoadFile( wpath.c_str(), nullptr, &sourceBlob );
    if( FAILED( hr ) )
    {
        LOG_INFO( std::format( "Failed to compile Shader: {}", mPath ) );
        return false;
    }

    DxcBuffer source = {};
    source.Ptr = sourceBlob->GetBufferPointer();
    source.Size = sourceBlob->GetBufferSize();
    source.Encoding = DXC_CP_UTF8;

    // コンパイルオプション
    auto wprofile = StringHelper::Convert( mProfile );
    LPCWSTR arguments[] =
        {
            wpath.c_str(),
            L"-E",
            L"main",
            L"-T",
            wprofile.c_str(),
            L"-Zi",
            L"-Qembed_debug",
            L"-Od",
            L"-Zpr",
        };

    // コンパイル
    Microsoft::WRL::ComPtr<IDxcResult> result = nullptr;
    hr = compiler->Compile( &source, arguments, _countof( arguments ), includeHandler.Get(), IID_PPV_ARGS( &result ) );
    if( FAILED( hr ) )
    {
        LOG_INFO( std::format( "Failed to compile Shader: {}", mPath ) );
        return false;
    }

    // エラーを出力
    Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors = nullptr;
    result->GetOutput( DXC_OUT_ERRORS, IID_PPV_ARGS( &errors ), nullptr );
    if( errors && errors->GetStringLength() > 0 )
    {
        LOG_ERROR( errors->GetStringPointer() );
        LOG_INFO( std::format( "Failed to compile Shader: {}", mPath ) );
        return false;
    }

    // シェーダーオブジェクトを取得
    hr = result->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( &mBlob ), nullptr );
    if( FAILED( hr ) ) return false;

    LOG_INFO( std::format( "Shader compiled successfully: {}", mPath ) );
    return true;
}

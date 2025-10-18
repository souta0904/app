#include "ShaderObject.h"

#include <format>

#include "utils/Logger.h"
#include "utils/StringHelper.h"

// コンストラクタ
ShaderObject::ShaderObject()
    : mState( State::Unload )
    , mTask()
    , mPath()
    , mProfile()
    , mBlob( nullptr )
{
}

// コンパイル
void ShaderObject::CompileAsync(
    const std::string& path,
    const std::string& profile,
    Microsoft::WRL::ComPtr<IDxcUtils> utils,
    Microsoft::WRL::ComPtr<IDxcCompiler3> compiler,
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler )
{
    mState = State::Load;
    mPath = path;
    mProfile = profile;

    mTask = std::async(
        std::launch::async,
        [this, utils, compiler, includeHandler]()
        {
            try
            {
                if( !utils || !compiler || !includeHandler )
                {
                    throw std::runtime_error( std::format( "Failed to compile Shader: {}", mPath ) );
                }

                // シェーダーファイルを読み込む
                auto wpath = StringHelper::Convert( mPath );
                Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob = nullptr;
                auto hr = utils->LoadFile( wpath.c_str(), nullptr, &sourceBlob );
                if( FAILED( hr ) )
                {
                    throw std::runtime_error( std::format( "Failed to compile Shader: {}", mPath ) );
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
                    throw std::runtime_error( std::format( "Failed to compile Shader: {}", mPath ) );
                }

                // エラーを出力
                Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors = nullptr;
                result->GetOutput( DXC_OUT_ERRORS, IID_PPV_ARGS( &errors ), nullptr );
                if( errors && errors->GetStringLength() > 0 )
                {
                    LOG_ERROR( errors->GetStringPointer() );
                    throw std::runtime_error( std::format( "Failed to compile Shader: {}", mPath ) );
                }

                // シェーダーオブジェクトを取得
                hr = result->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( &mBlob ), nullptr );
                if( FAILED( hr ) )
                {
                    throw std::runtime_error( std::format( "Failed to compile Shader: {}", mPath ) );
                }

                mState = State::Ready;
                LOG_INFO( std::format( "Shader compiled successfully: {}", mPath ) );
            }
            catch( const std::exception& e )
            {
                mState = State::Error;
                LOG_ERROR( e.what() );
            }
        } );
}

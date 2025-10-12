#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>

#include <string>

// シェーダーオブジェクト
class ShaderObject
{
   private:
    // シェーダーのパス
    std::wstring mPath;
    // プロファイル
    std::wstring mProfile;
    // シェーダーオブジェクト
    Microsoft::WRL::ComPtr<IDxcBlob> mBlob;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    ShaderObject();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ShaderObject() = default;

    /// <summary>
    /// コンパイル
    /// </summary>
    /// <param name="path">シェーダーのパス</param>
    /// <param name="profile">プロファイル</param>
    /// <param name="utils">DXCユーティリティ</param>
    /// <param name="compiler">DXCコンパイラ</param>
    /// <param name="includeHandler">インクルードハンドラ</param>
    /// <returns>成否</returns>
    bool Compile(
        const std::wstring& path,
        const std::wstring& profile,
        Microsoft::WRL::ComPtr<IDxcUtils> utils,
        Microsoft::WRL::ComPtr<IDxcCompiler3> compiler,
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler );

    /// <summary>シェーダーオブジェクトを取得</summary>
    Microsoft::WRL::ComPtr<IDxcBlob> GetBlob() const { return mBlob; }
};

#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>

#include <future>
#include <string>

// シェーダーオブジェクト
class ShaderObject
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

    // シェーダーのパス
    std::string mPath;
    // プロファイル
    std::string mProfile;
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
    void CompileAsync(
        const std::string& path,
        const std::string& profile,
        Microsoft::WRL::ComPtr<IDxcUtils> utils,
        Microsoft::WRL::ComPtr<IDxcCompiler3> compiler,
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler );

    /// <summary>使用可能か</summary>
    bool IsReady() { return mState == State::Ready; }

    /// <summary>非同期タスクを取得</summary>
    std::future<void>& GetTask() { return mTask; }

    /// <summary>シェーダーオブジェクトを取得</summary>
    Microsoft::WRL::ComPtr<IDxcBlob> GetBlob() const { return mBlob; }
};

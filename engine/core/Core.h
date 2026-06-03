#pragma once
#include "Window.h"

namespace Nebula
{

/// <summary>
/// 基盤
/// </summary>
class Core
{
   private:
    Window* mWindow;

   public:
    Core();
    ~Core() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="windowWidth">ウィンドウの幅</param>
    /// <param name="windowHeight">ウィンドウの高さ</param>
    /// <param name="windowTitle">タイトルバーの文字</param>
    /// <returns>成否</returns>
    bool Initialize( uint32_t windowWidth, uint32_t windowHeight, const std::wstring& windowTitle );

    /// <summary>
    /// 実行
    /// </summary>
    void Run();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Terminate();
};

}  // namespace Nebula

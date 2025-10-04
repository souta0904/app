#include "EditorBase.h"

#include "DirectXBase.h"
#include "Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"

// フォントのパス
const std::string EditorBase::kFontPath = "assets/font/CascadiaCode.ttf";
// フォントのサイズ
const float EditorBase::kFontSize = 14.0f;

// コンストラクタ
EditorBase::EditorBase()
    : mSRVHdl( nullptr )
{
}

// 初期化
bool EditorBase::Init()
{
    auto& window = Window::GetInstance();
    auto& dxBase = DirectXBase::GetInstance();

    if( !ImGui::CreateContext() ) return false;

    // フォント
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF( kFontPath.c_str(), kFontSize, nullptr, io.Fonts->GetGlyphRangesJapanese() );
    if( !ImGui_ImplWin32_Init( window.GetHWnd() ) ) return false;

    auto* srvHeap = dxBase.GetSRVHeap();
    if( !srvHeap ) return false;

    mSRVHdl = srvHeap->Alloc();
    if( !mSRVHdl ) return false;

    if( !ImGui_ImplDX12_Init( dxBase.GetDevice().Get(), DirectXBase::kBackBuffCount, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap->GetDescriptorHeap().Get(), mSRVHdl->mCPU, mSRVHdl->mGPU ) ) return false;

    // XXX: フォントアトラスがビルドされていないらしい
    unsigned char* pixels;
    int w, h;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &w, &h );

    return true;
}

// 終了処理
void EditorBase::Term()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    auto* srvHeap = DirectXBase::GetInstance().GetSRVHeap();
    if( !srvHeap ) return;

    srvHeap->Free( mSRVHdl );
}

// エディタ開始
void EditorBase::Begin()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

// エディタ終了
void EditorBase::End()
{
    ImGui::Render();
}

// 描画
void EditorBase::Draw( CommandList* cmdList )
{
    ImGui_ImplDX12_RenderDrawData( ImGui::GetDrawData(), cmdList->GetCmdList().Get() );
}

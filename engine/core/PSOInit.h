#pragma once
#include <d3d12.h>

#include <vector>

class RootSignature;
class ShaderObject;

/// <summary>
/// GraphicsPSO作成用
/// </summary>
struct GraphicsPSOInit
{
    // ルートシグネチャ
    RootSignature* mRootSignature = nullptr;
    // 頂点シェーダー
    ShaderObject* mVS = nullptr;
    // ピクセルシェーダー
    ShaderObject* mPS = nullptr;
    // ブレンド設定
    D3D12_BLEND_DESC mBlendState = {};
    // ラスタライザ設定
    D3D12_RASTERIZER_DESC mRasterizerState = {};
    // 深度ステンシル設定
    D3D12_DEPTH_STENCIL_DESC mDepthStencilState = {};
    // 頂点レイアウト
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
    // プリミティブ型
    D3D12_PRIMITIVE_TOPOLOGY_TYPE mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    // レンダーターゲット
    UINT mNumRenderTargets = 1;
    DXGI_FORMAT mRTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB };
    // 深度ステンシル
    DXGI_FORMAT mDSVFormat = DXGI_FORMAT_D32_FLOAT;
};

/// <summary>
/// ComputePSO作成用
/// </summary>
struct ComputePSOInit
{
    // ルートシグネチャ
    RootSignature* mRootSignature = nullptr;
    // コンピュートシェーダー
    ShaderObject* mCS = nullptr;
};

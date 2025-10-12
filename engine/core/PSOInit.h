#pragma once
#include <d3d12.h>

#include <vector>

class RootSignature;
class ShaderObject;

/// <summary>
/// PSO初期化構造体
/// </summary>
struct PSOInit
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
};

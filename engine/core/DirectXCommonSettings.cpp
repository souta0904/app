#include "DirectXCommonSettings.h"

namespace DirectXCommonSettings
{

// ヒーププロパティ
D3D12_HEAP_PROPERTIES gHeapDefault = {};
D3D12_HEAP_PROPERTIES gHeapUpload = {};

// ブレンド設定
D3D12_BLEND_DESC gBlendNone = {};
D3D12_BLEND_DESC gBlendAlpha = {};
D3D12_BLEND_DESC gBlendAdd = {};
D3D12_BLEND_DESC gBlendSubtract = {};
D3D12_BLEND_DESC gBlendMultiply = {};
D3D12_BLEND_DESC gBlendScreen = {};

// ラスタライザ設定
D3D12_RASTERIZER_DESC gRasterizerDefault = {};

// 深度ステンシル設定
D3D12_DEPTH_STENCIL_DESC gDepthDefault = {};

// サンプラー設定
D3D12_STATIC_SAMPLER_DESC gSamplerLinearClamp = {};

// 初期化
bool Init()
{
    // ヒーププロパティ
    gHeapDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
    gHeapUpload.Type = D3D12_HEAP_TYPE_UPLOAD;

    // ブレンド設定
    gBlendNone.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // アルファ
    gBlendAlpha.AlphaToCoverageEnable = false;
    gBlendAlpha.IndependentBlendEnable = false;
    gBlendAlpha.RenderTarget[0].BlendEnable = true;
    gBlendAlpha.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    gBlendAlpha.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    gBlendAlpha.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    gBlendAlpha.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    gBlendAlpha.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    gBlendAlpha.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    gBlendAlpha.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // 加算
    gBlendAdd = gBlendAlpha;
    gBlendAdd.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    gBlendAdd.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    gBlendAdd.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

    // 減算
    gBlendSubtract = gBlendAlpha;
    gBlendSubtract.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    gBlendSubtract.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    gBlendSubtract.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;

    // 乗算
    gBlendMultiply = gBlendAlpha;
    gBlendMultiply.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
    gBlendMultiply.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
    gBlendMultiply.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

    // スクリーン
    gBlendScreen = gBlendAlpha;
    gBlendScreen.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
    gBlendScreen.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    gBlendScreen.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

    // ラスタライザ設定
    gRasterizerDefault.FillMode = D3D12_FILL_MODE_SOLID;
    gRasterizerDefault.CullMode = D3D12_CULL_MODE_BACK;
    gRasterizerDefault.DepthClipEnable = true;

    // 深度ステンシル設定
    gDepthDefault.DepthEnable = true;
    gDepthDefault.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    gDepthDefault.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    // サンプラー設定
    gSamplerLinearClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    gSamplerLinearClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    gSamplerLinearClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    gSamplerLinearClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    gSamplerLinearClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    gSamplerLinearClamp.MaxLOD = D3D12_FLOAT32_MAX;
    gSamplerLinearClamp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    return true;
}

// 終了処理
void Term()
{
}

}  // namespace DirectXCommonSettings

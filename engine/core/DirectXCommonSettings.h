#pragma once
#include <d3d12.h>

/// <summary>
/// DirectX共通の設定
/// </summary>
namespace DirectXCommonSettings
{

// ヒーププロパティ
extern D3D12_HEAP_PROPERTIES gHeapDefault;
extern D3D12_HEAP_PROPERTIES gHeapUpload;

// ブレンド設定
extern D3D12_BLEND_DESC gBlendNone;
extern D3D12_BLEND_DESC gBlendAlpha;
extern D3D12_BLEND_DESC gBlendAdd;
extern D3D12_BLEND_DESC gBlendSubtract;
extern D3D12_BLEND_DESC gBlendMultiply;
extern D3D12_BLEND_DESC gBlendScreen;

// ラスタライザ設定
extern D3D12_RASTERIZER_DESC gRasterizerDefault;
extern D3D12_RASTERIZER_DESC gRasterizerNoCulling;

// 深度ステンシル設定
extern D3D12_DEPTH_STENCIL_DESC gDepthDefault;
extern D3D12_DEPTH_STENCIL_DESC gDepthDisable;

// サンプラー設定
extern D3D12_STATIC_SAMPLER_DESC gSamplerLinearWrap;

/// <summary>
/// 初期化
/// </summary>
/// <returns></returns>
bool Init();

/// <summary>
/// 終了処理
/// </summary>
void Term();

};  // namespace DirectXCommonSettings

#pragma once
#include <d3d12.h>

/// <summary>
/// DirectX共通の設定
/// </summary>
namespace DirectXCommonSettings
{

// ヒーププロパティ
extern D3D12_HEAP_PROPERTIES gHeapPropertiesDefault;
extern D3D12_HEAP_PROPERTIES gHeapPropertiesUpload;

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

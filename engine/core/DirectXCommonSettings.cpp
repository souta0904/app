#include "DirectXCommonSettings.h"

namespace DirectXCommonSettings
{

// ヒーププロパティ
D3D12_HEAP_PROPERTIES gHeapPropertiesDefault = {};
D3D12_HEAP_PROPERTIES gHeapPropertiesUpload = {};

// 初期化
bool Init()
{
    // ヒーププロパティ
    gHeapPropertiesDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
    gHeapPropertiesUpload.Type = D3D12_HEAP_TYPE_UPLOAD;

    return true;
}

// 終了処理
void Term()
{
}

}  // namespace DirectXCommonSettings

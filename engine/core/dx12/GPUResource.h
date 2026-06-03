#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace Nebula
{

/// <summary>
/// GPUリソース
/// </summary>
class GPUResource
{
   protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;

   public:
    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return mResource; }
};

}  // namespace Nebula

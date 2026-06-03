#pragma once
#include <cstdint>

#include "GPUResource.h"

struct DescriptorHandle;
class DirectXBase;

namespace Nebula
{

/// <summary>
/// 深度バッファ
/// </summary>
class DepthBuffer : public GPUResource
{
   private:
    DescriptorHandle* mDSVHdl;

   public:
    DepthBuffer();
    ~DepthBuffer() = default;

    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="dxBase">DirectX基盤</param>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    /// <returns>成否</returns>
    bool Create( DirectXBase* dxBase, uint32_t width, uint32_t height );

    DescriptorHandle* GetDSVHdl() const { return mDSVHdl; }
};

}  // namespace Nebula

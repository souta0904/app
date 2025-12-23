#pragma once
#include "utils/EnumFlags.h"

/// <summary>
/// メッシュフラグ
/// </summary>
enum class MeshFlags : uint32_t
{
    None = 0,
    Position = 1 << 0,
    Normal = 1 << 1,
    UV = 1 << 2,

    Required = Position | Normal | UV,  // 必須
};
ENABLE_ENUM_FLAGS( MeshFlags )

/// <summary>
/// マテリアルフラグ
/// </summary>
enum class MaterialFlags : uint32_t
{
    None = 0,
    HasTexture = 1 << 0,
    NoCulling = 1 << 1,
    Wireframe = 1 << 2,
};
ENABLE_ENUM_FLAGS( MaterialFlags )

/// <summary>
/// メッシュフラグとマテリアルフラグからGraphicsPSOのキーを作成
/// </summary>
/// <param name="meshFlags">メッシュフラグ</param>
/// <param name="materialFlags">マテリアルフラグ</param>
/// <returns>作成したキー</returns>
constexpr uint64_t MakePSOKey( MeshFlags meshFlags, MaterialFlags materialFlags )
{
    return ( static_cast<uint64_t>( meshFlags ) << 32 ) | static_cast<uint64_t>( materialFlags );
}

/// <summary>
/// GraphicsPSOのキーからメッシュフラグを取得
/// </summary>
/// <param name="psoKey">GraphicsPSOのキー</param>
/// <returns>メッシュフラグ</returns>
constexpr MeshFlags GetMeshFlags( uint64_t psoKey )
{
    return static_cast<MeshFlags>( psoKey >> 32 );
}

/// <summary>
/// GraphicsPSOのキーからマテリアルフラグを取得
/// </summary>
/// <param name="psoKey">GraphicsPSOのキー</param>
/// <returns>マテリアルフラグ</returns>
constexpr MaterialFlags GetMaterialFlags( uint64_t psoKey )
{
    return static_cast<MaterialFlags>( psoKey & 0xFFFFFFFFull );
}

#pragma once

/// <summary>
/// レンダーキュー
/// </summary>
enum class RenderQueue
{
    Opaque = 1000,
    Transparent = 2000,
    Max,
};

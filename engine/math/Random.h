#pragma once
#include <random>

#include "Vector2.h"
#include "Vector3.h"

/// <summary>
/// ランダム
/// </summary>
class Random
{
   private:
    // メルセンヌ・ツイスタ
    static std::mt19937 sEngine;

   public:
    /// <summary>
    /// 初期化
    /// </summary>
    static void Init();

    /// <summary>
    /// [min,max)の乱数を生成
    /// </summary>
    static int32_t Next( int32_t min, int32_t max );

    /// <summary>
    /// [min,max)の乱数を生成
    /// </summary>
    static float Next( float min, float max );

    /// <summary>
    /// [min,max)の乱数を生成
    /// </summary>
    static Vector2 Next( const Vector2& min, const Vector2& max );

    /// <summary>
    /// [min,max)の乱数を生成
    /// </summary>
    static Vector3 Next( const Vector3& min, const Vector3& max );
};

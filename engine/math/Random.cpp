#include "Random.h"

std::mt19937 Random::sEngine;

// 初期化
void Random::Init()
{
    std::random_device rd;
    sEngine.seed( rd() );
}

// [min,max)の乱数を生成
int32_t Random::Next( int32_t min, int32_t max )
{
    std::uniform_int_distribution<int32_t> dist( min, max );
    return dist( sEngine );
}

// [min,max)の乱数を生成
float Random::Next( float min, float max )
{
    std::uniform_real_distribution<float> dist( min, max );
    return dist( sEngine );
}

// [min,max)の乱数を生成
Vector2 Random::Next( const Vector2& min, const Vector2& max )
{
    auto vec = Vector2(
        min.x + ( max.x - min.x ) * Next( 0.0f, 1.0f ),
        min.y + ( max.y - min.y ) * Next( 0.0f, 1.0f ) );
    return vec;
}

// [min,max)の乱数を生成
Vector3 Random::Next( const Vector3& min, const Vector3& max )
{
    auto vec = Vector3(
        min.x + ( max.x - min.x ) * Next( 0.0f, 1.0f ),
        min.y + ( max.y - min.y ) * Next( 0.0f, 1.0f ),
        min.z + ( max.z - min.z ) * Next( 0.0f, 1.0f ) );
    return vec;
}

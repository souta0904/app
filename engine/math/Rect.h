#pragma once

/// <summary>
/// 矩形
/// </summary>
class Rect
{
   public:
    float left;
    float top;
    float right;
    float bottom;

    Rect()
        : left( 0.0f )
        , top( 0.0f )
        , right( 0.0f )
        , bottom( 0.0f )
    {
    }

    Rect( float left, float top, float right, float bottom )
        : left( left )
        , top( top )
        , right( right )
        , bottom( bottom )
    {
    }
};

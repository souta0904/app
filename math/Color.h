#pragma once

/// <summary>
/// 色
/// </summary>
class Color
{
   public:
    static const Color kBlack;
    static const Color kGray;
    static const Color kWhite;
    static const Color kRed;
    static const Color kGreen;
    static const Color kBlue;
    static const Color kCyan;
    static const Color kMagenta;
    static const Color kYellow;

    float r;
    float g;
    float b;
    float a;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Color()
        : r( 0.0f )
        , g( 0.0f )
        , b( 0.0f )
        , a( 1.0f )
    {
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="r">赤</param>
    /// <param name="g">緑</param>
    /// <param name="b">青</param>
    /// <param name="a">不透明度</param>
    Color( float r, float g, float b, float a = 1.0f )
        : r( r )
        , g( g )
        , b( b )
        , a( a )
    {
    }
};

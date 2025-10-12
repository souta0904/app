#include "StringHelper.h"

#include <Windows.h>

namespace StringHelper
{

// マルチバイト文字をワイド文字へ変換
std::wstring Convert( const std::string& str )
{
    if( str.empty() )
    {
        return std::wstring();
    }

    // 必要なサイズを取得
    auto sizeNeeded = MultiByteToWideChar( CP_UTF8, 0, str.data(), static_cast<int>( str.size() ), nullptr, 0 );
    if( sizeNeeded < 1 )
    {
        return std::wstring();
    }

    // ワイド文字へ変換
    std::wstring result( sizeNeeded, 0 );
    MultiByteToWideChar( CP_UTF8, 0, str.data(), static_cast<int>( str.size() ), result.data(), sizeNeeded );
    return result;
}

// ワイド文字をマルチバイト文字へ変換
std::string Convert( const std::wstring& str )
{
    if( str.empty() )
    {
        return std::string();
    }

    // 必要なサイズを取得
    auto sizeNeeded = WideCharToMultiByte( CP_UTF8, 0, str.data(), static_cast<int>( str.size() ), nullptr, 0, nullptr, nullptr );
    if( sizeNeeded < 1 )
    {
        return std::string();
    }

    // マルチバイト文字へ変換
    std::string result( sizeNeeded, 0 );
    WideCharToMultiByte( CP_UTF8, 0, str.data(), static_cast<int>( str.size() ), result.data(), sizeNeeded, nullptr, nullptr );
    return result;
}

}  // namespace StringHelper

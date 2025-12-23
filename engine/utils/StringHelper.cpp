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

// パスから親パスを取得
std::string GetParentPath( const std::string& path )
{
    auto pos = path.find_last_of( "/\\" );
    if( pos != std::string::npos )
    {
        return path.substr( 0, pos );
    }
    return "";
}

// パスからファイル名を取得
std::string GetFilename( const std::string& path )
{
    auto pos = path.find_last_of( "/\\" );
    if( pos != std::string::npos )
    {
        return path.substr( pos + 1 );
    }
    return path;
}

// パスから拡張子を除いたファイル名を取得
std::string GetStem( const std::string& path )
{
    auto filename = GetFilename( path );
    auto pos = filename.find_last_of( "." );
    if( pos != std::string::npos )
    {
        return filename.substr( 0, pos );
    }
    return filename;
}

// パスから拡張子を取得
std::string GetExtension( const std::string& path )
{
    auto filename = GetFilename( path );
    auto pos = filename.find_last_of( "." );
    if( pos != std::string::npos )
    {
        return filename.substr( pos );
    }
    return "";
}

}  // namespace StringHelper

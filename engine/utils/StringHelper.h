#pragma once
#include <string>

/// <summary>
/// 文字列関連ヘルパー
/// </summary>
namespace StringHelper
{

/// <summary>
/// マルチバイト文字をワイド文字へ変換
/// </summary>
/// <param name="str">マルチバイト文字</param>
/// <returns>変換したワイド文字</returns>
std::wstring Convert( const std::string& str );

/// <summary>
/// ワイド文字をマルチバイト文字へ変換
/// </summary>
/// <param name="str">ワイド文字</param>
/// <returns>変換したマルチバイト文字</returns>
std::string Convert( const std::wstring& str );

/// <summary>
/// パスから親パスを取得
/// </summary>
/// <param name="path">対象パス</param>
/// <returns>親パス</returns>
std::string GetParentPath( const std::string& path );

/// <summary>
/// パスからファイル名を取得
/// </summary>
/// <param name="path">対象パス</param>
/// <returns>ファイル名</returns>
std::string GetFilename( const std::string& path );

/// <summary>
/// パスから拡張子を除いたファイル名を取得
/// </summary>
/// <param name="path">対象パス</param>
/// <returns>拡張子を除いたファイル名</returns>
std::string GetStem( const std::string& path );

/// <summary>
/// パスから拡張子を取得
/// </summary>
/// <param name="path">対象パス</param>
/// <returns>拡張子</returns>
std::string GetExtension( const std::string& path );

};  // namespace StringHelper

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

};  // namespace StringHelper

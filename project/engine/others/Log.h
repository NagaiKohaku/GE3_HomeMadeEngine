#pragma once
#include <Windows.h>
#include <string>
#include <format>

/// <summary>
/// デバッグログを出力する関数
/// </summary>
void OutPutLog(const std::string& message);

/// <summary>
/// std::stringをstd::wstringに変換する関数
/// </summary>
std::wstring ConvertString(const std::string& str);

/// <summary>
//std::wstringをstd::stringに変換する関数
/// </summary>
std::string ConvertString(const std::wstring& str);
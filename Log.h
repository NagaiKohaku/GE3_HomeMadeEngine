#pragma once
#include <Windows.h>
#include <string>
#include <format>

class Log {

public:

	/// <summary>
	/// デバッグログを出力する関数
	/// </summary>
	static void OutPut(const std::string& message);

	/// <summary>
	/// std::stringをstd::wstringに変換する関数
	/// </summary>
	static std::wstring ConvertString(const std::string& str);

	/// <summary>
	//std::wstringをstd::stringに変換する関数
	/// </summary>
	static std::string ConvertString(const std::wstring& str);
};
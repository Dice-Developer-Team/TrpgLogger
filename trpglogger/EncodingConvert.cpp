#include "EncodingConvert.h"
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// ±àÂë×ª»»
std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, nullptr, 0);
	const auto wideStr = new wchar_t[len + 1];
	memset(wideStr, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wideStr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wideStr, -1, str, len, nullptr, nullptr);
	std::string strTemp(str);
	delete[] wideStr;
	delete[] str;
	return strTemp;
}

std::string GBKToUTF8(const std::string& strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, nullptr, 0);
	const auto wideStr = new wchar_t[len + 1];
	memset(wideStr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, wideStr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, str, len, nullptr, nullptr);
	std::string strTemp(str);
	delete[] wideStr;
	delete[] str;
	return strTemp;
}
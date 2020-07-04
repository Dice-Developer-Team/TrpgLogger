#pragma once
/**
 * 字符串辅助函数
 * Copyright (C) 2019-2020 String.Empty
 */

#include <string>

using std::string;
using std::wstring;
using std::to_string;

#define CP_GB18030 (54936)

string toString(int num, unsigned short size = 0);

template <typename Dig>
string to_signed_string(Dig num)
{
	if (num > 0)return "+" + to_string(num);
	return to_string(num);
}

int count_char(const string& s, char ch);

string convert_w2a(const wchar_t* wch);

wstring convert_a2w(const char* ch);

#pragma once
#ifndef TRPGLOGGER_ENCODING_CONVERT
#define TRPGLOGGER_ENCODING_CONVERT
#include <string>

// UTF8编码转换为GBK编码
std::string UTF8ToGBK(const std::string& strUTF8);

// GBK编码转换为UTF8编码
std::string GBKToUTF8(const std::string& strGBK);

#endif /*TRPGLOGGER_ENCODING_CONVERT*/
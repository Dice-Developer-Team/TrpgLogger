#pragma once
#ifndef TRPGLOGGER_GLOBALVAR
#define TRPGLOGGER_GLOBALVAR

#include <string>
#include <map>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/Aws.h>

// 全局应用目录
extern std::string fileLoc;
// 全局数据库路径
extern std::string dbLoc_UTF8;
// Aws SDK设置
extern Aws::SDKOptions options;
// Aws Credentials
extern Aws::Auth::AWSCredentials awsCredentials;
// 应用是否启用
extern bool Enabled;
// Log信息存储
extern std::map<long long, time_t> LogInfo;			// 群
extern std::map<long long, time_t> LogInfoDiscuss;	// 讨论组
// 版本信息
extern const std::string TrpgLoggerVer;
//自定义回执
extern std::string CustomReplyLoc_UTF8;
extern std::map<std::string, std::string> CustomReply;
extern const std::vector<std::string> CustomReplyFilter;
//静态替换
std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);

#endif /*TRPGLOGGER_GLOBALVAR*/
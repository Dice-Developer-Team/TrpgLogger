#include "GlobalVar.h"
#include <string>
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>

// 全局应用目录
std::string fileLoc;
// 全局数据库路径
std::string dbLoc_UTF8;
// Aws SDK设置
Aws::SDKOptions options;
// Aws Credentials, 显式指定使用空字符串匿名Credentials
Aws::Auth::AWSCredentials awsCredentials("", "");
// 应用是否启用
bool Enabled = false;
// Log信息存储
std::map<long long, time_t> LogInfo;		// 群
std::map<long long, time_t> LogInfoDiscuss;	// 讨论组
// 版本信息
const std::string TrpgLoggerVer = "TrpgLogger by w4123溯洄 Version 1.0.3(17)";

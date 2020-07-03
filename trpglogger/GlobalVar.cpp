#include "GlobalVar.h"
#include <string>
#include <map>
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
const std::string TrpgLoggerVer = "TrpgLogger by w4123溯洄 Version 1.0.4(18)";
//自定义回执
std::string CustomReplyLoc_UTF8;
std::map<std::string, std::string> CustomReply
{
	{"AlreadyLogging", "正在进行日志记录, 无法再次开始!"},
	{"StartLogging", "开始日志记录"},
	{"StartSaveLog", "正在保存日志"},
	{"SuccessSaveLog", "日志记录已结束，文件已保存，正在上传至服务器"},
	{"FailSaveLog", "保存失败，您可以尝试稍后重试此命令再次保存\n错误信息:"},
	{"SuccessUploadLogBefore", "上传已完成，请访问"},
	{"SuccessUploadLogAfter", "以查看记录"},
	{"FailUploadLog", "上传过程中发生错误，请联系管理员或稍后再次使用此命令重试上传\n错误信息:"},
	{"NeverLog", "没有已开始的日志记录!"},
	{"LogHlp", "\n.log     \t 启动日志记录\n.log stop\t 停止日志记录\n.log help\t 日志记录帮助"}
};
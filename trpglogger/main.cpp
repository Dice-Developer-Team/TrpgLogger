// SNS.cpp : 定义 DLL 应用程序的导出函数。
//

#include <map>
#include <ctime>
#include <future>
#include <string>
#include <fstream>
#include <aws/core/Aws.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "CQEVE_ALL.h"
#include "APPINFO.h"
#include "S3PutObject.h"
#include "GlobalVar.h"
#include "SaveLog.h"
#include "MsgType.h"
#include "EncodingConvert.h"

EVE_Enable(eventEnable)
{
	// 设置应用为已启用
	Enabled = true;

	// 初始化Aws API
	Aws::InitAPI(options);

	// 获取应用目录
	fileLoc = CQ::getAppDirectory();

	// 获取可执行文件路径
	char fileName[256];
	const int ret = GetModuleFileNameA(nullptr, fileName, 256);
	if (ret == 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		return -1;
	}

	// 由可执行文件路径获取日志数据库路径
	std::string tempLoc(fileName);
	dbLoc_UTF8 = GBKToUTF8(tempLoc.substr(0, tempLoc.find_last_of('\\')) + "\\data\\" + std::to_string(CQ::getLoginQQ()) + "\\eventv2.db");

	std::ifstream readSessionGroup(fileLoc + "Group.session");
	if (readSessionGroup)
	{
		long long groupNum;
		time_t startTime;
		while (readSessionGroup >> groupNum >> startTime)
		{
			LogInfo[groupNum] = startTime;
		}
	}
	readSessionGroup.close();

	std::ifstream readSessionDiscuss(fileLoc + "Discuss.session");
	if (readSessionDiscuss)
	{
		long long discussNum;
		time_t startTime;
		while (readSessionDiscuss >> discussNum >> startTime)
		{
			LogInfoDiscuss[discussNum] = startTime;
		}
	}
	readSessionDiscuss.close();
	return 0;
}

// 群聊部分
EVE_GroupMsg_EX(eventGroupMsg)
{
	std::string strAt = "[CQ:at,qq=" + std::to_string(CQ::getLoginQQ()) + "]";
	if (eve.message.substr(0, 6) == "[CQ:at")
	{
		if (eve.message.substr(0, strAt.length()) != strAt)
		{
			return;
		}
		eve.message = eve.message.substr(strAt.length());
	}
	while (eve.message[0] == ' ')
	{
		eve.message.erase(eve.message.begin());
	}
	if (eve.message.substr(0, 2) == "。")
	{
		eve.message.erase(eve.message.begin());
		eve.message[0] = '.';
	}
	std::transform(eve.message.begin(), eve.message.end(), eve.message.begin(), [](unsigned char c) { return tolower(c); });
	if (eve.message == ".log" || eve.message == ".logstart" || eve.message == ".log start" || eve.message == ".log on" || eve.message == ".logon")
	{
		eve.message_block();
		if (LogInfo.count(eve.fromGroup))
		{
			eve.sendMsg("正在进行日志记录, 无法再次开始!");
		}
		else
		{
			eve.sendMsg("开始日志记录");
			LogInfo[eve.fromGroup] = time(nullptr);
		}
	}
	else if (eve.message == ".log stop" || eve.message == ".logstop" || eve.message == ".logend" || eve.message == ".log end" || eve.message == ".log off" || eve.message == ".logoff" || eve.message == ".log fin" || eve.message == ".logfin")
	{
		eve.message_block();
		if (LogInfo.count(eve.fromGroup))
		{
			// 开始保存日志
			eve.sendMsg("正在保存日志");

			// 读取记录信息
			const time_t time_start(LogInfo[eve.fromGroup]);
			LogInfo.erase(eve.fromGroup);

			// 获取记录结束时间
			const time_t now = time(nullptr);

			// 拼接文件保存地址
			const std::string fileName = "group_" + std::to_string(eve.fromGroup) + "_" + std::to_string(time_start) + ".txt";
			const std::string logLoc = fileLoc + fileName;

			// 保存文件
			const std::string save_log_res = saveLog(time_start, now, eve.fromGroup, MsgType::Group, logLoc);
			if (save_log_res=="SUCCESS")
			{
				eve.sendMsg("日志记录已结束，文件已保存，正在上传至服务器");
			}
			else
			{
				eve.sendMsg(save_log_res);
				return;
			}
			
			// 上传文件
			const std::string put_obj_res = put_s3_object("dicelogger", fileName, logLoc, "ap-southeast-1");
			if (put_obj_res == "SUCCESS")
			{
				eve.sendMsg("上传已完成，请访问 https://logpainter.kokona.tech/?s3=" + fileName + " 以查看记录");
			}
			else
			{
				eve.sendMsg("上传过程中发生错误，请联系管理员\n错误信息: " + put_obj_res);
			}
			
		}
		else
		{
			eve.sendMsg("没有已开始的日志记录!");
		}
	}
	else if (eve.message == ".log help" || eve.message == ".loghelp")
	{
		eve.message_block();
		eve.sendMsg(TrpgLoggerVer + "\n.log     \t 启动日志记录\n.log stop\t 停止日志记录\n.log help\t 日志记录帮助");
	}
}

// 讨论组部分
EVE_DiscussMsg_EX(eventDiscussMsg)
{
	std::string strAt = "[CQ:at,qq=" + std::to_string(CQ::getLoginQQ()) + "]";
	if (eve.message.substr(0, 6) == "[CQ:at")
	{
		if (eve.message.substr(0, strAt.length()) != strAt)
		{
			return;
		}
		eve.message = eve.message.substr(strAt.length());
	}
	while (eve.message[0] == ' ')
	{
		eve.message.erase(eve.message.begin());
	}
	if (eve.message.substr(0, 2) == "。")
	{
		eve.message.erase(eve.message.begin());
		eve.message[0] = '.';
	}
	std::transform(eve.message.begin(), eve.message.end(), eve.message.begin(), [](unsigned char c) { return tolower(c); });
	if (eve.message == ".log" || eve.message == ".logstart" || eve.message == ".log start" || eve.message == ".log on" || eve.message == ".logon")
	{
		eve.message_block();
		if (LogInfoDiscuss.count(eve.fromDiscuss))
		{
			eve.sendMsg("正在进行日志记录, 无法再次开始!");
		}
		else
		{
			eve.sendMsg("开始日志记录");
			LogInfoDiscuss[eve.fromDiscuss] = time(nullptr);
		}
	}
	else if (eve.message == ".log stop" || eve.message == ".logstop" || eve.message == ".logend" || eve.message == ".log end" || eve.message == ".log off" || eve.message == ".logoff" || eve.message == ".log fin" || eve.message == ".logfin")
	{
		eve.message_block();
		if (LogInfoDiscuss.count(eve.fromDiscuss))
		{
			// 开始保存日志
			eve.sendMsg("正在保存日志");

			// 读取记录信息
			const time_t time_start(LogInfoDiscuss[eve.fromDiscuss]);
			LogInfoDiscuss.erase(eve.fromDiscuss);

			// 获取记录结束时间
			const time_t now = time(nullptr);

			// 拼接文件保存地址
			const std::string fileName = "discuss_" + std::to_string(eve.fromDiscuss) + "_" + std::to_string(time_start) + ".txt";
			const std::string logLoc = fileLoc + fileName;

			// 保存文件
			const std::string save_log_res = saveLog(time_start, now, eve.fromDiscuss, MsgType::Discuss, logLoc);
			if (save_log_res == "SUCCESS")
			{
				eve.sendMsg("日志记录已结束，文件已保存，正在上传至服务器");
			}
			else
			{
				eve.sendMsg(save_log_res);
				return;
			}

			// 上传文件
			const std::string put_obj_res = put_s3_object("dicelogger", fileName, logLoc, "ap-southeast-1");
			if (put_obj_res == "SUCCESS")
			{
				eve.sendMsg("上传已完成，请访问 https://logpainter.kokona.tech/?s3=" + fileName + " 以查看记录");
			}
			else
			{
				eve.sendMsg("上传过程中发生错误，请联系管理员\n错误信息: " + put_obj_res);
			}

		}
		else
		{
			eve.sendMsg("没有已开始的日志记录!");
		}
	}
	else if (eve.message == ".log help" || eve.message == ".loghelp")
	{
		eve.message_block();
		eve.sendMsg(TrpgLoggerVer + "\n.log     \t 启动日志记录\n.log stop\t 停止日志记录\n.log help\t 日志记录帮助");
	}
}

// 私聊消息部分, 仅返回帮助信息
EVE_PrivateMsg_EX(eventPrivateMsg)
{
	if (eve.message.substr(0, 2) == "。")
	{
		eve.message.erase(eve.message.begin());
		eve.message[0] = '.';
	}
	std::transform(eve.message.begin(), eve.message.end(), eve.message.begin(), [](unsigned char c) { return tolower(c); });
	if(eve.message.substr(0,4) == ".log")
	{
		eve.message_block();
		eve.sendMsg(TrpgLoggerVer + "\n.log     \t 启动日志记录\n.log stop\t 停止日志记录\n.log help\t 日志记录帮助\n请注意命令仅会在群/讨论组中生效");
	}
}

EVE_Disable(eventDisable)
{
	// 设置应用为未启用
	Enabled = false;

	// 释放Aws API资源
	Aws::ShutdownAPI(options);

	// 保存Session信息并清空内存中的信息
	std::ofstream saveSessionGroup(fileLoc + "Group.session", std::ios::out | std::ios::trunc);
	if (saveSessionGroup)
	{
		for (const auto& ele : LogInfo)
		{
			saveSessionGroup << ele.first << " " << ele.second << std::endl;
		}
	}
	saveSessionGroup.close();
	LogInfo.clear();

	std::ofstream saveSessionDiscuss(fileLoc + "Discuss.session", std::ios::out | std::ios::trunc);
	if (saveSessionDiscuss)
	{
		for (const auto& ele : LogInfoDiscuss)
		{
			saveSessionDiscuss << ele.first << " " << ele.second << std::endl;
		}
	}
	saveSessionDiscuss.close();
	LogInfoDiscuss.clear();
	return 0;
}

EVE_Exit(eventExit)
{
	// 如果应用未启用, 则在eventDisable中已经释放过资源了，无需再次释放
	if (Enabled)
	{
		return eventDisable();
	}
	return 0;
}

MUST_AppInfo_RETURN(CQAPPID)
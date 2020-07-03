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
#include "CJsonObject.hpp"

void save();

EVE_Enable(eventEnable)
{
	// 设置应用为已启用
	Enabled = true;

	// 初始化Aws API
	Aws::InitAPI(options);

	// 获取应用目录
	fileLoc = CQ::getAppDirectory();

	// 获取可执行文件路径
	char fileName[MAX_PATH];
	const int ret = GetModuleFileNameA(nullptr, fileName, MAX_PATH);
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

	// 由可执行文件路径获取自定义回执路径
	CustomReplyLoc_UTF8 = GBKToUTF8(tempLoc.substr(0, tempLoc.find_last_of('\\')) + "\\LogData\\CustomReply.json");

	//加载自定义回执配置文件
	std::ifstream CustomReplyJSON(CustomReplyLoc_UTF8, std::ios::in);
	std::stringstream CustomReplyJSONsstr;
	CustomReplyJSONsstr << CustomReplyJSON.rdbuf();
	neb::CJsonObject CustomReplyJSONobj(UTF8ToGBK(CustomReplyJSONsstr.str()));

	//加载自定义回执
	CustomReply["self"] = CQ::getLoginNick();
	for (auto &it : CustomReply)
	{
		CustomReplyJSONobj.Get(it.first, CustomReply[it.first]);
		replace_all(CustomReply[it.first], "{self}", CustomReply["self"]);
	}

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
			eve.sendMsg(CustomReply["AlreadyLogging"]);
		}
		else
		{
			eve.sendMsg(CustomReply["StartLogging"]);
			LogInfo[eve.fromGroup] = time(nullptr);
			save();
		}
	}
	else if (eve.message == ".log stop" || eve.message == ".logstop" || eve.message == ".logend" || eve.message == ".log end" || eve.message == ".log off" || eve.message == ".logoff" || eve.message == ".log fin" || eve.message == ".logfin")
	{
		eve.message_block();
		if (LogInfo.count(eve.fromGroup))
		{
			// 开始保存日志
			eve.sendMsg(CustomReply["StartSaveLog"]);

			// 读取记录信息
			const time_t time_start(LogInfo[eve.fromGroup]);

			// 获取记录结束时间
			const time_t now = time(nullptr);

			// 拼接文件保存地址
			const std::string fileName = "group_" + std::to_string(eve.fromGroup) + "_" + std::to_string(time_start) + ".txt";
			const std::string logLoc = fileLoc + fileName;

			// 保存文件
			const std::string save_log_res = saveLog(time_start, now, eve.fromGroup, MsgType::Group, logLoc);
			if (save_log_res=="SUCCESS")
			{
				eve.sendMsg(CustomReply["SuccessSaveLog"]);
			}
			else
			{
				eve.sendMsg(CustomReply["FailSaveLog"] + " " + save_log_res);
				return;
			}
			
			// 上传文件
			const std::string put_obj_res = put_s3_object("dicelogger", fileName, logLoc, "ap-southeast-1");
			if (put_obj_res == "SUCCESS")
			{
				eve.sendMsg(CustomReply["SuccessUploadLogBefore"] + " https://logpainter.kokona.tech/?s3=" + fileName + " " + CustomReply["SuccessUploadLogAfter"]);
				LogInfo.erase(eve.fromGroup);
				save();
			}
			else
			{
				eve.sendMsg(CustomReply["FailUploadLog"] + " " + put_obj_res);
			}

		}
		else
		{
			eve.sendMsg(CustomReply["NeverLog"]);
		}
	}
	else if (eve.message == ".log help" || eve.message == ".loghelp")
	{
		eve.message_block();
		eve.sendMsg(TrpgLoggerVer + CustomReply["LogHlp"]);
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
			eve.sendMsg(CustomReply["AlreadyLogging"]);
		}
		else
		{
			eve.sendMsg(CustomReply["StartLogging"]);
			LogInfoDiscuss[eve.fromDiscuss] = time(nullptr);
			save();
		}
	}
	else if (eve.message == ".log stop" || eve.message == ".logstop" || eve.message == ".logend" || eve.message == ".log end" || eve.message == ".log off" || eve.message == ".logoff" || eve.message == ".log fin" || eve.message == ".logfin")
	{
		eve.message_block();
		if (LogInfoDiscuss.count(eve.fromDiscuss))
		{
			// 开始保存日志
			eve.sendMsg(CustomReply["StartSaveLog"]);

			// 读取记录信息
			const time_t time_start(LogInfoDiscuss[eve.fromDiscuss]);

			// 获取记录结束时间
			const time_t now = time(nullptr);

			// 拼接文件保存地址
			const std::string fileName = "discuss_" + std::to_string(eve.fromDiscuss) + "_" + std::to_string(time_start) + ".txt";
			const std::string logLoc = fileLoc + fileName;

			// 保存文件
			const std::string save_log_res = saveLog(time_start, now, eve.fromDiscuss, MsgType::Discuss, logLoc);
			if (save_log_res == "SUCCESS")
			{
				eve.sendMsg(CustomReply["SuccessSaveLog"]);
			}
			else
			{
				eve.sendMsg(CustomReply["FailSaveLog"] + " " + save_log_res);
				return;
			}

			// 上传文件
			const std::string put_obj_res = put_s3_object("dicelogger", fileName, logLoc, "ap-southeast-1");
			if (put_obj_res == "SUCCESS")
			{
				eve.sendMsg(CustomReply["SuccessUploadLogBefore"] + " https://logpainter.kokona.tech/?s3=" + fileName + " " + CustomReply["SuccessUploadLogAfter"]);
				LogInfoDiscuss.erase(eve.fromDiscuss);
				save();
			}
			else
			{
				eve.sendMsg(CustomReply["FailUploadLog"] + " " + put_obj_res);
			}

		}
		else
		{
			eve.sendMsg(CustomReply["NeverLog"]);
		}
	}
	else if (eve.message == ".log help" || eve.message == ".loghelp")
	{
		eve.message_block();
		eve.sendMsg(TrpgLoggerVer + CustomReply["LogHlp"]);
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
		eve.sendMsg(TrpgLoggerVer + CustomReply["LogHlp"]);
	}
}
void save()
{
	std::ofstream saveSessionGroup(fileLoc + "Group.session", std::ios::out | std::ios::trunc);
	if (saveSessionGroup)
	{
		for (const auto& ele : LogInfo)
		{
			saveSessionGroup << ele.first << " " << ele.second << std::endl;
		}
	}
	saveSessionGroup.close();

	std::ofstream saveSessionDiscuss(fileLoc + "Discuss.session", std::ios::out | std::ios::trunc);
	if (saveSessionDiscuss)
	{
		for (const auto& ele : LogInfoDiscuss)
		{
			saveSessionDiscuss << ele.first << " " << ele.second << std::endl;
		}
	}
	saveSessionDiscuss.close();
}
EVE_Disable(eventDisable)
{
	// 设置应用为未启用
	Enabled = false;

	// 释放Aws API资源
	Aws::ShutdownAPI(options);

	// 保存Session信息并清空内存中的信息
	save();
	LogInfo.clear();
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
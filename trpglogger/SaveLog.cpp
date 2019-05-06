#include "SaveLog.h"

#include <ctime>
#include <fstream>

#include <CQTools.h>

#include "GlobalVar.h"
#include "SQLiteCpp/Database.h"
#include "CQAPI_EX.h"
#include "EncodingConvert.h"
#include "MsgType.h"


const std::string SQL = "SELECT account, content, time FROM event WHERE ([group] = ? AND time >= ? AND time <= ? AND (type = 2 OR type = 2001))";

// 保存记录到文件
std::string saveLog(time_t time_start, time_t now, long long groupNum, MsgType msgType, const std::string& logLoc)
{
	// 创建文件保存输出流
	std::ofstream saveStream(logLoc);
	if (!saveStream)
	{
		return "保存日志失败!\n错误信息: 创建文件保存输出流失败!";
	}

	try
	{
		// 创建日志数据库对象及语句，绑定参数
		SQLite::Database db(dbLoc_UTF8, SQLite::OPEN_READONLY, 3000);
		SQLite::Statement query(db, SQL);
		if (msgType == MsgType::Group)
		{
			query.bind(1, "qq/group/" + std::to_string(groupNum));
		}
		else
		{
			query.bind(1, "qq/discuss/" + std::to_string(groupNum));
		}
		query.bind(2, time_start - 3); // 保证所有内容都被录入
		query.bind(3, now + 3); // 保证所有内容都被录入

		// 获取日志数据库内的消息信息并保存
		while (query.executeStep())
		{
			// 获取消息信息
			const std::string account = query.getColumn(0);
			std::string msg = query.getColumn(1);
			const time_t time = query.getColumn(2);

			// 获取QQ号
			const long long account_num = account.empty() ? CQ::getLoginQQ() :
				std::stoll(account.substr(account.find_last_of('/') + 1));

			// 格式化时间信息
			tm tm_info{};
			localtime_s(&tm_info, &time);
			char tm_buffer[10];
			strftime(tm_buffer, 10, "%H:%M:%S", &tm_info);
			
			// 消息处理(替换@ image & 解码)
			size_t at_left = msg.find("[CQ:at");
			size_t at_right = msg.find(']', at_left + 1);
			while (at_left != std::string::npos && at_right != std::string::npos)
			{
				
				if (msg.substr(at_left, at_right - at_left + 1) == "[CQ:at,id=all]")
				{
					msg.replace(at_left, at_right - at_left + 1, u8"@全体成员");
				}
				else
				{
					std::string qq_num(msg.begin() + at_left + 18, msg.begin() + at_right);
					long long qqNum = std::stoll(qq_num);
					if (msgType==MsgType::Group)
					{
						msg.replace(at_left, at_right - at_left + 1, u8"@" + GBKToUTF8(!CQ::getGroupMemberInfo(groupNum, qqNum).GroupNick.empty() ? CQ::getGroupMemberInfo(groupNum, qqNum).GroupNick : CQ::getStrangerInfo(qqNum).nick));
					}
					else
					{
						msg.replace(at_left, at_right - at_left + 1, u8"@" + GBKToUTF8(CQ::getStrangerInfo(qqNum).nick));
					}
				}
				
				at_left = msg.find("[CQ:at");
				at_right = msg.find(']', at_left + 1);
			}

			at_left = msg.find("[CQ:image");
			at_right = msg.find(']', at_left + 1);
			while (at_left != std::string::npos && at_right != std::string::npos)
			{
				msg.replace(at_left, at_right - at_left + 1, u8"&#91;图片&#93;");
				at_left = msg.find("[CQ:image");
				at_right = msg.find(']', at_left + 1);
			}
			msg_decode(msg);

			// 保存日志
			if (msgType == MsgType::Group)
			{
				saveStream << GBKToUTF8(!CQ::getGroupMemberInfo(groupNum, account_num).GroupNick.empty() ? CQ::getGroupMemberInfo(groupNum, account_num).GroupNick : CQ::getStrangerInfo(account_num).nick);
			}
			else
			{
				saveStream << GBKToUTF8(CQ::getStrangerInfo(account_num).nick);
			}
			
			saveStream << '(' << account_num << ") ";
			saveStream << tm_buffer << std::endl;
			saveStream << msg << std::endl << std::endl;
		}
	}
	catch (std::exception & e)
	{
		return std::string("保存日志失败!\n错误信息: ") + e.what();
	}

	// 关闭文件输出流
	saveStream.close();
	return "SUCCESS";
}

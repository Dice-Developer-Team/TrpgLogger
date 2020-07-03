#pragma once
#ifndef TRPGLOGGER_GLOBALVAR
#define TRPGLOGGER_GLOBALVAR

#include <string>
#include <map>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/Aws.h>

// ȫ��Ӧ��Ŀ¼
extern std::string fileLoc;
// ȫ�����ݿ�·��
extern std::string dbLoc_UTF8;
// Aws SDK����
extern Aws::SDKOptions options;
// Aws Credentials
extern Aws::Auth::AWSCredentials awsCredentials;
// Ӧ���Ƿ�����
extern bool Enabled;
// Log��Ϣ�洢
extern std::map<long long, time_t> LogInfo;			// Ⱥ
extern std::map<long long, time_t> LogInfoDiscuss;	// ������
// �汾��Ϣ
extern const std::string TrpgLoggerVer;
//�Զ����ִ
extern std::string CustomReplyLoc_UTF8;
extern std::map<std::string, std::string> CustomReply;
//��̬�滻
std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);

#endif /*TRPGLOGGER_GLOBALVAR*/
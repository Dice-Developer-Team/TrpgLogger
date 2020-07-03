#include "GlobalVar.h"
#include <string>
#include <map>
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>


// ȫ��Ӧ��Ŀ¼
std::string fileLoc;
// ȫ�����ݿ�·��
std::string dbLoc_UTF8;
// Aws SDK����
Aws::SDKOptions options;
// Aws Credentials, ��ʽָ��ʹ�ÿ��ַ�������Credentials
Aws::Auth::AWSCredentials awsCredentials("", "");
// Ӧ���Ƿ�����
bool Enabled = false;
// Log��Ϣ�洢
std::map<long long, time_t> LogInfo;		// Ⱥ
std::map<long long, time_t> LogInfoDiscuss;	// ������
// �汾��Ϣ
const std::string TrpgLoggerVer = "TrpgLogger by w4123��� Version 1.0.4(18)";
//�Զ����ִ
std::string CustomReplyLoc_UTF8;
std::map<std::string, std::string> CustomReply
{
	{"self", "��"},
	{"strAlreadyLogging", "���ڽ�����־��¼, �޷��ٴο�ʼ!"},
	{"strStartLogging", "��ʼ��־��¼"},
	{"strStartSaveLog", "���ڱ�����־"},
	{"strSuccessSaveLog", "��־��¼�ѽ������ļ��ѱ��棬�����ϴ���������"},
	{"strFailSaveLog", "����ʧ�ܣ������Գ����Ժ����Դ������ٴα���\n������Ϣ:"},
	{"strSuccessUploadLogBefore", "�ϴ�����ɣ������"},
	{"strSuccessUploadLogAfter", "�Բ鿴��¼"},
	{"strFailUploadLog", "�ϴ������з�����������ϵ����Ա���Ժ��ٴ�ʹ�ô����������ϴ�\n������Ϣ:"},
	{"strNeverLog", "û���ѿ�ʼ����־��¼!"},
	{"strLogHlp", "\n.log     \t ������־��¼\n.log stop\t ֹͣ��־��¼\n.log help\t ��־��¼����"}
};
//��̬�滻
std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
{
	while (true)
	{
		std::string::size_type pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
	return str;
}
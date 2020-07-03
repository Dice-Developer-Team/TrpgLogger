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
	{"AlreadyLogging", "���ڽ�����־��¼, �޷��ٴο�ʼ!"},
	{"StartLogging", "��ʼ��־��¼"},
	{"StartSaveLog", "���ڱ�����־"},
	{"SuccessSaveLog", "��־��¼�ѽ������ļ��ѱ��棬�����ϴ���������"},
	{"FailSaveLog", "����ʧ�ܣ������Գ����Ժ����Դ������ٴα���\n������Ϣ:"},
	{"SuccessUploadLogBefore", "�ϴ�����ɣ������"},
	{"SuccessUploadLogAfter", "�Բ鿴��¼"},
	{"FailUploadLog", "�ϴ������з�����������ϵ����Ա���Ժ��ٴ�ʹ�ô����������ϴ�\n������Ϣ:"},
	{"NeverLog", "û���ѿ�ʼ����־��¼!"},
	{"LogHlp", "\n.log     \t ������־��¼\n.log stop\t ֹͣ��־��¼\n.log help\t ��־��¼����"}
};
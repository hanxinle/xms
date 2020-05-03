#pragma once
#ifdef _WIN32
#ifdef XCOM_EXPORTS
#define XCOM_API __declspec(dllexport)
#else
#define XCOM_API __declspec(dllimport)
#endif
#else
#define XCOM_API
#endif
#include <string>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>
#include <list>
#include "xlog_client.h"
using namespace std;

XCOM_API std::string GetDirData(std::string path);

struct XToolFileInfo
{
    std::string filename = "";
    long long filesize = 0;
    bool is_dir = false;
    long long time_write = 0;// �޸ĵ�ʱ���
    std::string time_str = "";      //�޸�ʱ���ַ��� 2020-01-22 19:30:13
};

//��ȡĿ¼�б�   //��ʽ �ļ������ļ���С��byte�����Ƿ���Ŀ¼��0,1�����ļ��޸�ʱ�䣨2020-01-22 19:30:13����
XCOM_API std::list< XToolFileInfo > GetDirList(std::string path);

//ɾ���ļ�
XCOM_API void XDelFile(std::string path);

//����Ŀ¼
XCOM_API void XNewDir(std::string path);


XCOM_API void XStringSplit(std::vector<std::string> &vec, std::string str, std::string find);


XCOM_API void XStrReplace(std::string str, std::string str_find, std::string str_replace, std::string &sreturn);


XCOM_API std::string XFormatDir(const std::string &dir);

XCOM_API std::string XTrim(const std::string& s);

XCOM_API bool XFileExist(const std::string& s);

/////////////////////////////////////////////////////////////
///����md5 128bit(16�ֽ�) 
///@para in_data ��������
///@para in_data_size ���������ֽ���
///@para out_md �����MD5���� ��16�ֽڣ�
XCOM_API unsigned char *XMD5(const unsigned char *in_data, unsigned long in_data_size, unsigned char *out_md);

/////////////////////////////////////////////////////////////
///����md5_base64  (24�ֽ�) �پ���base64ת��Ϊ�ַ���
///@para in_data ��������
///@para in_data_size ���������ֽ���
///@return  �����MD5 base64 ���� ��24�ֽڣ�
XCOM_API std::string XMD5_base64(const unsigned char *in_data, unsigned long in_data_size);


/////////////////////////////////////////////////////////////
///����base64 ���ر�����ֽڴ�С
XCOM_API  int Base64Encode(const unsigned char *in, int len, char *out_base64);

/////////////////////////////////////////////////////////////
///����base64 ���ؽ����ֽڴ�С
XCOM_API  int Base64Decode(const char *in, int len, unsigned char *out_data);


/*
%a ���ڼ��ļ�д
%A ���ڼ���ȫ��
%b �·ֵļ�д
%B �·ݵ�ȫ��
%c ��׼�����ڵ�ʱ�䴮
%C ��ݵĺ���λ����
%d ʮ���Ʊ�ʾ��ÿ�µĵڼ���
%D ��/��/��
%e �����ַ����У�ʮ���Ʊ�ʾ��ÿ�µĵڼ���
%F ��-��-��
%g ��ݵĺ���λ���֣�ʹ�û����ܵ���
%G ��֣�ʹ�û����ܵ���
%h ��д���·���
%H 24Сʱ�Ƶ�Сʱ
%I 12Сʱ�Ƶ�Сʱ
%j ʮ���Ʊ�ʾ��ÿ��ĵڼ���
%m ʮ���Ʊ�ʾ���·�
%M ʮʱ�Ʊ�ʾ�ķ�����
%n ���з�
%p ���ص�AM��PM�ĵȼ���ʾ
%r 12Сʱ��ʱ��
%R ��ʾСʱ�ͷ��ӣ�hh:mm
%S ʮ���Ƶ�����
%t ˮƽ�Ʊ��
%T ��ʾʱ���룺hh:mm:ss
%u ÿ�ܵĵڼ��죬����һΪ��һ�� ��ֵ��0��6������һΪ0��
%U ����ĵڼ��ܣ�����������Ϊ��һ�죨ֵ��0��53��
%V ÿ��ĵڼ��ܣ�ʹ�û����ܵ���
%w ʮ���Ʊ�ʾ�����ڼ���ֵ��0��6��������Ϊ0��
%W ÿ��ĵڼ��ܣ�������һ��Ϊ��һ�죨ֵ��0��53��
%x ��׼�����ڴ�
%X ��׼��ʱ�䴮
%y �������͵�ʮ������ݣ�ֵ��0��99��
%Y �����Ͳ��ֵ�ʮ�����
%z��%Z ʱ�����ƣ�������ܵõ�ʱ�������򷵻ؿ��ַ���
%% �ٷֺ�����ĳ�������ʾ��ǰ���������ڣ�
*/
XCOM_API std::string XGetTime(int timestamp = 0, std::string fmt = "%F %R");

XCOM_API std::string XGetSizeString(long long size);

XCOM_API std::string XGetIconFilename(std::string filename,bool is_dir=false);

//ͨ������ ����IP��ַ ֻȡ��һ��
// windows C:\Windows\System32\drivers\etc
// Linux /etc/hosts
// 127.0.0.1 xms_gateway_server
// 127.0.0.1 xms_register_server
XCOM_API std::string XGetHostByName(std::string host_name);



//�õ�Ŀ¼�ֽڴ�С
XCOM_API long long GetDirSize(const char * path);

/////////////////////////////////////////////////////////////////////
/// �õ����̿ռ��С
/// @para dir ����·�� C:/test   /root
/// @para avail �û����ÿռ� �ֽ���
/// @para total ���̿ռ� �ֽ���
/// @para free ����ʣ��ռ� �ֽ���
XCOM_API bool GetDiskSize(const char *dir, unsigned long long *avail, unsigned long long *total, unsigned long long *free);



class XMsg;
namespace xmsg
{
    class XMsgHead;
}
XCOM_API  void PrintMsg(xmsg::XMsgHead *head, XMsg *msg);
#define XMUTEX(s) XMutex tmp_mutex(s,#s)
class XCOM_API XMutex
{
public:
     static bool is_debug;
     XMutex(std::mutex *mux);
     XMutex(std::mutex *mux, std::string msg);
     ~XMutex();
private:
    int index_ = 0;
    std::string msg_ = "";
    std::mutex *mux_ = 0;

};

//AES ��Կ
//if (bits != 128 && bits != 192 && bits != 256)


class XCOM_API XAES
{
public:
    static XAES* Create();
    ///////////////////////////////////////////////////////
    /// ���ü�����Կ ��Կ���� 128λ��16�ֽڣ� 192λ ��24�ֽڣ� 256λ (32�ֽ�)
    /// ���Ȳ��ܳ���32�ֽڣ�����ʧ��
    /// ��Կ�����Զ�����
    /// @key ��Կ
    /// @key_size ��Կ���� �ֽ� <=32 ���Զ�����Կ
    /// @is_enc true  ���� false ����
    /// @return ���óɹ�ʧ��
    virtual bool SetKey(const char *key, int key_byte_size, bool is_enc) = 0;
    
    ///����ռ䣬ɾ������
    virtual void Drop() = 0;

    ///////////////////////////////////////////////////////
    /// �ӽ���
    /// @in ��������
    /// @in_size �������ݴ�С
    /// @out ��� ���ݿռ�Ҫ��֤16�ֽڵı���
    /// @return  �����С��ʧ�ܷ���<=0
    virtual long long Decrypt(const unsigned char *in, long long in_size, unsigned char *out) = 0;
    virtual long long Encrypt(const unsigned char *in, long long in_size, unsigned char *out) = 0;
};
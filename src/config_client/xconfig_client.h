#pragma once
#include "xservice_client.h"

//ǰ������
namespace google
{
    namespace protobuf 
    {
        namespace compiler
        {
            class Importer;
            class DiskSourceTree;
        }
    }
}

typedef void(*ConfigTimerCBFunc) ();
class XConfigClient :public XServiceClient
{
public:
    ~XConfigClient();
    static XConfigClient *Get()
    {
        static XConfigClient cc;
        return &cc;
    }

    ConfigTimerCBFunc ConfigTimerCB = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///�����������ģ���ʼ��ʱ����ȡ����
    bool StartGetConf(const char *local_ip, int local_port,
        google::protobuf::Message *conf_message, ConfigTimerCBFunc func);

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///�����������ģ���ʼ��ʱ����ȡ����
    ///@para local_ip  ����IP�ĵ�ַ���������Ϊ�մ�����NULL����Ĭ��ʹ�����ӵĵ�ַ��
    bool StartGetConf(const char *server_ip, int server_port,
        const char *local_ip, int local_port,
        google::protobuf::Message *conf_message, int timeout_sec = 10);

    ///��ȡ���صı��ز���
    int GetInt(const char *key);
    bool GetBool(const char *key);
    std::string GetString(const char *key);

    ///��ʱ���ص� ��ʱ��ȡ��������
    virtual void TimerCB();

    ////�ȵ���һ�ζ�ʱ����ȷ����Ϣ��ʱ��ȡ
    virtual bool Init();

    ///�ȴ��߳��˳� 
    void Wait();
   
    /////////////////////////////////////////////////////////////////////
    ///������ȡ�������� IP���ΪNULL ��ȡ�����������ĵĵ�ַ
    void LoadConfig(const char *ip, int port);

    ///��ȡ�����б��ѻ��棩�е����ã��Ḵ��һ�ݵ�out_conf
    bool GetConfig(const char *ip, int port, xmsg::XConfig *out_conf,int timeout_ms = 100);

    ///��Ӧ��ȡ�Լ�������  �洢������
    void LoadConfigRes(xmsg::XMsgHead *head, XMsg *msg);

    //���õ�ǰ���õĶ���
    void SetCurServiceMessage(google::protobuf::Message *message);

    /////////////////////////////////////////////////////////////////////
    //// ����proto�ļ� �̲߳���ȫ
    ///@para filename �ļ�·��
    ///@para class_name ���õ�����
    ///@para out_proto_code ��ȡ���Ĵ��룬�����ռ�Ͱ汾
    ///@return ���ض�̬������message �����ʧ�ܷ���NULL���ڶ��ε��û��ͷ���һ�οռ�
    google::protobuf::Message *LoadProto(std::string filename, std::string class_name,std::string &out_proto_code);


    static void RegMsgCallback()
    {
        //RegCB(xmsg::MSG_SAVE_CONFIG_RES, (MsgCBFunc)&XConfigClient::SendConfigRes);
        RegCB(xmsg::MSG_LOAD_CONFIG_RES, (MsgCBFunc)&XConfigClient::LoadConfigRes);
        //RegCB(xmsg::MSG_LOAD_ALL_CONFIG_RES, (MsgCBFunc)&XConfigClient::LoadAllConfigRes);
        //RegCB(xmsg::MSG_DEL_CONFIG_RES, (MsgCBFunc)&XConfigClient::DeleteConfigRes);
    }



private:
    xmsg::XLoginRes login_;
    std::mutex login_mutex_;
    XConfigClient();

    //����΢�����ip�Ͷ˿�
    char local_ip_[16] = { 0 };
    int local_port_ = 0;

    //��̬����proto�ļ�
    google::protobuf::compiler::Importer *importer_ = 0;

    //�����ļ��Ĺ������
    google::protobuf::compiler::DiskSourceTree *source_tree_ = 0;

    //����proto�ļ���̬�����ĵ�message
    google::protobuf::Message *message_ = 0;

    
};


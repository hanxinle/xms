#pragma once

#include "xservice_client.h"

typedef void(*ConfigResCBFunc) (bool is_ok, const char *msg);
typedef void(*GetConfigResCBFunc) (xmsg::XConfig);


#define MCONF XConfigManager::Get()
class XConfigManager :public XServiceClient
{
public:
    static XConfigManager *Get()
    {
        static XConfigManager cc;
        return &cc;
    }
    XConfigManager();
    ~XConfigManager();

    /////////////////////////////////////////////////////////////////////
    /// ɾ������
    //����ɾ����������
    void DeleteConfig(const char *ip, int port);

    //ɾ��������Ӧ
    void DeleteConfigRes(xmsg::XMsgHead *head, XMsg *msg);

    /////////////////////////////////////////////////////////////////////
    /// ��ȡȫ�������б�
    /// 1 �Ͽ������Զ�����
    /// 2 �ȴ��������
    xmsg::XConfigList GetAllConfig(int page, int page_count, int timeout_sec);

    ///��Ӧ��ȡ�����б�  �洢������
    void LoadAllConfigRes(xmsg::XMsgHead *head, XMsg *msg);

    /////////////////////////////////////////////////////////////////////
    //���ͱ�������
    void SendConfig(xmsg::XConfig *conf);

    //���յ��������õ���Ϣ
    void SendConfigRes(xmsg::XMsgHead *head, XMsg *msg);

    /////////////////////////////////////////////////////////////////////
    ///������ȡ�������� IP���ΪNULL ��ȡ�����������ĵĵ�ַ
    void LoadConfig(const char *ip, int port);

    ///��Ӧ��ȡ�Լ�������  �洢������
    void LoadConfigRes(xmsg::XMsgHead *head, XMsg *msg);

    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_SAVE_CONFIG_RES, (MsgCBFunc)&XConfigManager::SendConfigRes);
        RegCB(xmsg::MSG_LOAD_CONFIG_RES, (MsgCBFunc)&XConfigManager::LoadConfigRes);
        RegCB(xmsg::MSG_LOAD_ALL_CONFIG_RES, (MsgCBFunc)&XConfigManager::LoadAllConfigRes);
        RegCB(xmsg::MSG_DEL_CONFIG_RES, (MsgCBFunc)&XConfigManager::DeleteConfigRes);
    }


    //�趨��¼��Ϣ
    void set_login(xmsg::XLoginRes login);

    //�������ú�Ļص�����
    ConfigResCBFunc SendConfigResCB = 0;

    //�������ú�Ļص�
    GetConfigResCBFunc LoadConfigResCB = 0;




private:
    xmsg::XLoginRes login_;
    
    std::mutex login_mutex_;

    virtual bool  SendMsg(xmsg::MsgType type, const google::protobuf::Message *message);

   // xmsg::XConfig config_;
    //std::mutex config_mutex_;
};


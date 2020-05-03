#ifndef XREGISTER_CLIENT_H
#define XREGISTER_CLIENT_H
#include "xservice_client.h"

////////////////////////////////////////////////
//// ע�����Ŀͻ��� ��windows����ֱ�������ļ�
class XRegisterClient:public XServiceClient
{
public:
    static XRegisterClient *Get()
    {
        static XRegisterClient *xc = 0;
        if (!xc)
        {
            xc = new XRegisterClient();
        }
        return xc;
    }

    ~XRegisterClient();

    //���ӳɹ�����Ϣ�ص�����ҵ��������
    virtual void ConnectedCB();


    //���շ����ע����Ӧ
    void RegisterRes(xmsg::XMsgHead *head, XMsg *msg);

    //��ȡ�����б����Ӧ
    void GetServiceRes(xmsg::XMsgHead *head, XMsg *msg);

    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_REGISTER_RES, (MsgCBFunc)&XRegisterClient::RegisterRes);
        RegCB(xmsg::MSG_GET_SERVICE_RES, (MsgCBFunc)&XRegisterClient::GetServiceRes);
    }

    ///////////////////////////////////////////////////////////////////////
    //// ��ע������ע����� �˺�������Ҫ��һ�����ã���������
    /// @para service_name ΢��������
    /// @para port ΢����ӿ�
    /// @para ip ΢�����ip���������NULL������ÿͻ������ӵ�ַ
    /// @para is_find �Ƿ����Ϊ��������
    void RegisterServer(const char *service_name, int port, const char *ip, bool is_find = false);

    /// ��ȡ���еķ����б�����ԭ���ݣ�ÿ�������ϴεĸ�������
    /// �˺����Ͳ���XServiceMap���ݵĺ�����һ���߳�
    xmsg::XServiceMap *GetAllService();


    /////////////////////////////////////////////////////////////////////////////
    /// ��ȡָ���������Ƶ�΢�����б� ������������
    /// 1 �ȴ����ӳɹ� 2 ���ͻ�ȡ΢�������Ϣ 3 �ȴ�΢�����б���Ϣ�������п����õ���һ�ε����ã�
    /// @para service_name ��������
    /// @para timeout_sec ��ʱʱ��
    /// @return �����б�
    xmsg::XServiceList GetServcies(const char *service_name, int timeout_sec);

    /////////////////////////////////////////////////////////////
    ///�����л�ȡ΢�����б������
    ///@para service_name == NULL ��ȡȫ��
    void GetServiceReq(const char *service_name);

    //��ʱ�������ڷ�������
    virtual void TimerCB();
private:
    XRegisterClient();

    //��ȡ���ػ��� �̲߳���ȫ����Ҫ��service_map����
    bool LoadLocalFile();

    char service_name_[32] = {0};
    int service_port_ = 0;
    char service_ip_[16] = {0};

    //�Ƿ���Ա���������
    bool is_find_ = false;


};

#endif
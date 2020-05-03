#ifndef XSERVICEPROXYCLIENT
#define XSERVICEPROXYCLIENT
#include "xservice_client.h"
#include <map>


class XServiceProxyClient :public XServiceClient
{
public:

    static XServiceProxyClient* Create(std::string service_name);

    ~XServiceProxyClient();
    virtual void ReadCB(xmsg::XMsgHead *head, XMsg *msg);

    //�������ݣ���ӱ�ʶ
    virtual bool SendMsg(xmsg::XMsgHead *head, XMsg *msg, XMsgEvent *ev);

    //ע��һ���¼�
    void RegEvent(XMsgEvent *ev);
    void DelEvent(XMsgEvent *ev);

    //�Ƿ���Ա���������
    bool is_find() { return is_find_; }
    void set_is_find(bool is) { this->is_find_ = is; }
protected:

    XServiceProxyClient();
    bool is_find_ = false;
         
    //��Ϣת���Ķ���һ��proxy��Ӧ���XMsgEvent
    //��ָ���ֵ��Ϊ������Ҫ����64λ
    std::map<long long, XMsgEvent *> callback_task_;
    std::mutex callback_task_mutex_;
};

#endif


#include "xservice_proxy_client.h"
#include "xtools.h"
#include "xauth_proxy.h"
#include "xlog_client.h"
using namespace std;

XServiceProxyClient* XServiceProxyClient::Create(std::string service_name)
{
    if (service_name == AUTH_NAME)
    {
        return new XAuthProxy();
    }
    return new XServiceProxyClient();
}

bool XServiceProxyClient::SendMsg(xmsg::XMsgHead *head, XMsg *msg, XMsgEvent *ev)
{
    RegEvent(ev);
    head->set_msg_id((long long)ev);
    return XMsgEvent::SendMsg(head, msg);
}
void XServiceProxyClient::DelEvent(XMsgEvent *ev)
{
    XMutex mux(&callback_task_mutex_);
    callback_task_.erase((long long)ev);
}
//ע��һ���¼�
void XServiceProxyClient::RegEvent(XMsgEvent *ev)
{
    XMutex mux(&callback_task_mutex_);
    callback_task_[(long long)ev] = ev;
}
void XServiceProxyClient::ReadCB(xmsg::XMsgHead *head, XMsg *msg)
{
    if (!head || !msg)return;


    cout << "***************************************" << endl;
    cout << head->DebugString();
    //XMutex mux(&callback_task_mutex_);
    //ת����XRouterHandle
    //ÿ��XServiceProxyClient������ܹ�����XRouterHandle
    auto router = callback_task_.find(head->msg_id());
    if (router == callback_task_.end())
    {
        LOGDEBUG("callback_task_ can't find");
        return; 
    }
    // ���߳����⣿�� ͨ�� �� ���
    router->second->SendMsg(head, msg);
}

XServiceProxyClient::XServiceProxyClient()
{
}


XServiceProxyClient::~XServiceProxyClient()
{
}

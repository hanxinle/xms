#include "xrouter_handle.h"
#include "xtools.h"
#include "xservice_proxy.h"
#include "xauth_proxy.h"
#include "xlog_client.h"
#include <list>
#include <thread>
using namespace std;
using namespace xmsg;

//�洢��Ϣ�ķ��͵Ĺ���
//class RouterMsg
//{
//public:
//
//    //���͵���Ϣ
//    xmsg::XMsgHead send_head;
//    XMsg send_msg;
//
//    //���յ���Ϣ
//    xmsg::XMsgHead recv_head;
//    XMsg recv_msg;
//};
//
//class MsgServerThread 
//{
//public:
//    void Start()
//    {
//        thread th(&XServiceProxy::Main, this);
//        th.detach();
//    }
//    //�����߳����ڴ�����Ϣ
//    void Main()
//    {
//        while (!is_exit_)
//        {
//
//        }
//    }
//    void Stop()
//    {
//        is_exit_ = true;
//    }
//
//    void Send(xmsg::XMsgHead *head, XMsg *m, XMsgEvent *ev)
//    {
//        XMsg msg;
//        //msg
//
//    }
//private:
//    bool is_exit_ = false;
//    list<XMsg> msgs;
//    //map<string, RouterMsg>
//
//};
//static MsgServerThread msg_server;
void XRouterHandle::InitMsgServer()
{
    //msg_server.Start();
}

void XRouterHandle::CloseMsgServer()
{
   // msg_server.Stop();
}

bool XRouterHandle::SendMsg(xmsg::XMsgHead *head, XMsg *msg)
{
    bool re = XMsgEvent::SendMsg(head, msg);
    if(re)
        cout << "��Ϣ�ѻظ�" << head ->DebugString()<< endl;
    else
        cout << "��Ϣ�ظ��쳣" << head->DebugString() << endl;
    return re;
}

void XRouterHandle::ReadCB(xmsg::XMsgHead *head, XMsg *msg)
{
    //ת����Ϣ
    LOGDEBUG("XRouterHandle::ReadCB");
    static int i = 0;
    i++;
    cout << i <<"XRouterHandle::ReadCB" << head->DebugString();

    //��ȡtoken��Ϣ
    string token = head->token();
    string user = head->username();
    //�����CheckToken ��Ϣ���հ� msgҲ�ж���size=0
    // ��֤token�Ƿ���Ч���Ƿ����û�һ��
    //bool re = XAuthProxy::Get()->CheckToken(user,token);

    
    //�˺����̳߳ص��ã����������Ӱ��ͬ�߳�����
    //��֤��ϢȨ��

    //��Ȩ�ɹ��ٷ�����Ϣ
    if (!head)
        return;

    //�ų�����Ҫ��Ȩ����Ϣ ,��Ҫ�Ż�
    if (head->msg_type() != MSG_LOGIN_REQ && 
        !XAuthProxy::CheckToken(head))
    {
        LOGINFO(head->DebugString());
        //LOGINFO("XAuthProxy::CheckToken failed!");
        //��Ȩʧ�ܣ��ݲ�������Ҫ�ع��Ż��ظ�����
        return;
        //XMsgEvent::SendMsg(head, msg);
    }


    head->set_msg_id((long long)this);
    //msg->head = head;
    //msg_server.Send(head, msg, this);
    XServiceProxy::Get()->SendMsg(head, msg,this);
}
void XRouterHandle::Close()
{
	XServiceProxy::Get()->DelEvent(this);
    XMsgEvent::Close();
}


XRouterHandle::XRouterHandle()
{
}


XRouterHandle::~XRouterHandle()
{
}

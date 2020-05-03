#include "xservice_client.h"
#include "xlog_client.h"
#include <string>
#include "xtools.h"
using namespace xmsg;
using namespace std;

XServiceClient::~XServiceClient()
{
    XMUTEX(&login_mutex_);
    delete login_;
    login_ = NULL;
    //delete thread_pool_;
    //thread_pool_ = NULL;
}

void XServiceClient::set_login(xmsg::XLoginRes *login)
{
    XMUTEX(&login_mutex_);
    if (!login_)
    {
        login_ = new XLoginRes();
    }
    login_->CopyFrom(*login);
}
xmsg::XMsgHead *XServiceClient::SetHead(xmsg::XMsgHead *head)
{
    if (!head)
        return NULL;
    if (service_name_.empty())
    {
        LOGDEBUG("service_name_ is empty");
    }
    else if(head->service_name().empty())
    {
        head->set_service_name(service_name_);
    }
    XMUTEX(&login_mutex_);
    if (!login_)
        return head;

    //��Ҫ���ǵ�¼��Ϣ
    if (!login_->token().empty())
    {
        //LOGINFO(login_.DebugString());
        string token = login_->token();
        //char *tmp = new char[token.size() + 1];
        //strcpy(tmp, token.c_str());
        //string *t = new string;
        //*t = token;

        //head->set_allocated_token(t);
        head->set_token(token);
        head->set_username(login_->username().c_str());
        head->set_rolename(login_->rolename().c_str());
        //LOGINFO(head->DebugString());
    }
    return head;
}
//��ȡ����˷������ƺ͵�¼��Ϣ��head
//xmsg::XMsgHead XServiceClient::GetHeadByType(MsgType type)
//{
//    XMsgHead head;
//    //SetHead(&head);
//    return head;
//}
    //������Ϣ
bool XServiceClient::SendMsg(xmsg::XMsgHead *h, const google::protobuf::Message *message)
{
    XMsgHead head;
    head.CopyFrom(*h);
    SetHead(&head);
    return XMsgEvent::SendMsg(&head, message);
}
bool XServiceClient::SendMsg(MsgType type, const google::protobuf::Message *message)
{
    //XMsgHead head;
    //head.set_msg_type((MsgType)type);
    ////��Ҫ���ǵ�¼��Ϣ
    //if (!login_.token().empty())
    //{
    //    head.set_token(login_.token());
    //    head.set_username(login_.username());
    //    head.set_rolename(login_.rolename());
    //}
    //if (service_name_.empty())
    //{
    //    LOGDEBUG("service_name_ is empty");
    //}

    //head.set_service_name(service_name_);
    XMsgHead head;
    head.set_msg_type(type);
    SetHead(&head);
    return XMsgEvent::SendMsg(&head, message);
}
//�����ļ���
bool XServiceClient::SendMsg(xmsg::XMsgHead *head, XMsg *msg)
{
    if (!head || !msg)
    {
        LOGDEBUG("head or msg is null");
        return false;
    }
        
    ////��Ҫ���ǵ�¼��Ϣ
    //if (!login_.token().empty())
    //{
    //    head->set_token(login_.token());
    //    head->set_username(login_.username());
    //    head->set_rolename(login_.rolename());
    //}
    //if (service_name_.empty())
    //{
    //    LOGDEBUG("service_name_ is empty");
    //}

    //head->set_service_name(service_name_);
    SetHead(head);
    return XMsgEvent::SendMsg(head, msg);
}
void XServiceClient::StartConnect()
{
    thread_pool_->Init(1);
    thread_pool_->Dispatch(this);
    //�ͻ��˲��Զ����٣���Ҫ����
    set_auto_delete(false);
}

XServiceClient::XServiceClient()
{
    this->thread_pool_ = XThreadPoolFactory::Create();
}


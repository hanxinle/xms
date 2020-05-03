#ifndef XSERVICES_CLIENT_H
#define XSERVICES_CLIENT_H
#include "xmsg_event.h"
#include "xthread_pool.h"
#include <thread>

/*
ʹ��ʾ��

class XMyServiceClient :public XServiceClient
{
public:
    //������Ϣ�������
    void SendMsgToServer()
    {
        //���л�������protbuf��Ϣ
        XDirReq req;
        req.set_path("test");
        SendMsg(MSG_DIR_REQ, &req);
    }
    //���շ������Ϣ
    void SendConfigRes(xmsg::XMsgHead *head, XMsg *msg)
    {
        //�����л�protobuf��Ϣ
        XDirRes res;
        if (!res.ParseFromArray(msg->data, msg->size))
        {
            cout<<"res.ParseFromArray failed!"<<endl;
            return;
        }
    }
    //ע����շ���������Ϣ����
    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_SAVE_CONFIG_RES, (MsgCBFunc)&XMyServiceClient::SendConfigRes);
    }
};
int main()
{
    //ע����շ���������Ϣ����
    XMyServiceClient::RegMsgCallback();

    //�趨������IP�Ͷ˿�
    XMyServiceClient client;
    client.set_server_ip("127.0.0.1");
    client.set_server_port(20010);

    //�������������̳߳�
    client.StartConnect();

    //������Ϣ�������
    client.SendMsgToServer();

    XThreadPool::Wait();
    return 0;
}

**/

class XCOM_API XServiceClient :public XMsgEvent
{
public:
    XServiceClient();
    virtual ~XServiceClient();
    
    //////////////////////////////////////////////////
    /// ��������뵽�̳߳��У���������
    virtual void StartConnect();


    //������Ϣ
    virtual bool SendMsg(xmsg::XMsgHead *head, const google::protobuf::Message *message) override;


    //������Ϣ
    virtual bool SendMsg(xmsg::MsgType type, const google::protobuf::Message *message) override;

    //�����ļ���
    virtual bool SendMsg(xmsg::XMsgHead *head, XMsg *msg) override;

    virtual void set_service_name(std::string name) { service_name_ = name; }
    virtual void set_login(xmsg::XLoginRes *login);

    xmsg::XLoginRes *login() { return login_; }

protected:
    //��ȡ����˷������ƺ͵�¼��Ϣ��head ���ײ��� dll�͵������ڴ�ռ�����
    // xmsg::XMsgHead GetHeadByType(xmsg::MsgType type);



private:

    //����head�ĵ�¼��Ϣ
    xmsg::XMsgHead *SetHead(xmsg::XMsgHead *head);

    XThreadPool *thread_pool_ = 0;
    
    //΢��������
    std::string service_name_;

    //��¼��Ϣ
    xmsg::XLoginRes *login_ = 0;

    std::mutex login_mutex_;


};


#endif
#pragma once
#include "xservice_handle.h"

//////////////////////////
///����ע�����ĵĿͻ��� ��Ӧһ������
class XRegisterHandle:public XServiceHandle
{
public:
    XRegisterHandle();
    ~XRegisterHandle();

    //���շ����ע������
    void RegisterReq(xmsg::XMsgHead *head, XMsg *msg);


    //���շ���ķ�������
    void GetServiceReq(xmsg::XMsgHead *head, XMsg *msg);
    void HeartRes(xmsg::XMsgHead *head, XMsg *msg) {};
    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_HEART_REQ, (MsgCBFunc)&XRegisterHandle::HeartRes);
        RegCB(xmsg::MSG_REGISTER_REQ, (MsgCBFunc)&XRegisterHandle::RegisterReq);
        RegCB(xmsg::MSG_GET_SERVICE_REQ, (MsgCBFunc)&XRegisterHandle::GetServiceReq);
    }

   
};


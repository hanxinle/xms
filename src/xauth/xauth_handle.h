#pragma once
#include "xservice_handle.h"
class XAuthHandle:public XServiceHandle
{
public:
    XAuthHandle();
    ~XAuthHandle();

    //////////////////////////////////////////////////////////////////
    ///���յ�¼����
    void LoginReq(xmsg::XMsgHead *head, XMsg *msg);

    //////////////////////////////////////////////////////////////////
    ///���յ�¼����
    void CheckTokenReq(xmsg::XMsgHead *head, XMsg *msg);

    //////////////////////////////////////////////////////////////////
    ///��������û���Ϣ
    void AddUserReq(xmsg::XMsgHead *head, XMsg *msg);

    //////////////////////////////////////////////////////////////////
    ///�����޸�������Ϣ
    void ChangePasswordReq(xmsg::XMsgHead *head, XMsg *msg);

    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_LOGIN_REQ, (MsgCBFunc)&XAuthHandle::LoginReq);
        RegCB(xmsg::MSG_ADD_USER_REQ, (MsgCBFunc)&XAuthHandle::AddUserReq);
        RegCB(xmsg::MSG_CHANGE_PASSWORD_REQ, (MsgCBFunc)&XAuthHandle::ChangePasswordReq);
        RegCB(xmsg::MSG_CHANGE_PASSWORD_REQ, (MsgCBFunc)&XAuthHandle::CheckTokenReq);
    }
};


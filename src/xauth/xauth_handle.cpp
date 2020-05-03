#include "xauth_handle.h"
#include "xauth_dao.h"
#include "xtools.h"
#include <string>
using namespace std;
using namespace xmsg;

//////////////////////////////////////////////////////////////////
///���յ�¼����
void XAuthHandle::CheckTokenReq(xmsg::XMsgHead *head, XMsg *msg)
{
    XLoginRes res;
    XAuthDao::Get()->CheckToken(head, &res);
}

//////////////////////////////////////////////////////////////////
///���յ�¼����
void XAuthHandle::LoginReq(xmsg::XMsgHead *head, XMsg *msg)
{
    //��¼��ʱʱ�䣬�����Ϊ���������Ļ�ȡ
    int timeout_sec = 1800;
   
    
    //��¼����
    XLoginReq req;

    //��¼������Ϣ
    XLoginRes res;
    if (!req.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("LoginReq failed!");
        res.set_res(XLoginRes::ERROR);
        res.set_token("LoginReq ParseFromArray failed!");
        SendMsg(MSG_LOGIN_RES, &res);
        return;
    }
    //bool re = XAuthDao::Get()->Login(req.username(), req.password(), token, timeout_sec);
    bool re = XAuthDao::Get()->Login(&req,&res, timeout_sec);
    if (!re)
    {
        LOGDEBUG("XAuthDao::Get()->Login failed!");
        res.set_res(XLoginRes::ERROR);
        res.set_token("username or password failed!");
        //res.set_res(XLoginRes::ERROR);
        //res.set_token("XAuthDao::Get()->Login failed!");
        //SendMsg(MSG_LOGIN_RES, &res);
        //return;
    }
    //res.set_res(XLoginRes::OK);
    //res.set_token(token);
    head->set_msg_type(MSG_LOGIN_RES);
    SendMsg(head, &res);
    //SendMsg(MSG_LOGIN_RES, &res);
}


//////////////////////////////////////////////////////////////////
///��������û���Ϣ
void XAuthHandle::AddUserReq(xmsg::XMsgHead *head, XMsg *msg)
{
    XAddUserReq req;
    XMessageRes res;
    if (!req.ParseFromArray(msg->data, msg->size))
    {
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("XAddUserReq ParseFromArray failed!");
        SendMsg(MSG_ADD_USER_RES, &res);
        return;
    }
    bool re = XAuthDao::Get()->AddUser(&req);
    if (!re)
    {
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("XAuthDao::Get()->AddUser failed!");
        SendMsg(MSG_ADD_USER_RES, &res);
        return;
    }
    res.set_return_(XMessageRes::OK);
    res.set_msg("OK!");
    head->set_msg_type(MSG_ADD_USER_RES);
    SendMsg(head, &res);

}

//////////////////////////////////////////////////////////////////
///�����޸�������Ϣ
void XAuthHandle::ChangePasswordReq(xmsg::XMsgHead *head, XMsg *msg)
{
    XChangePasswordReq req;
    XMessageRes res;
    if (!req.ParseFromArray(msg->data, msg->size))
    {
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("XChangePasswordReq ParseFromArray failed!");
        SendMsg(MSG_CHANGE_PASSWORD_RES, &res);
        return;
    }
    bool re = XAuthDao::Get()->ChangePassword(&req);
    if (!re)
    {
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("XAuthDao::Get()->ChangePassword failed!");
        SendMsg(MSG_CHANGE_PASSWORD_RES, &res);
        return;
    }
    res.set_return_(XMessageRes::OK);
    res.set_msg("OK!");

    head->set_msg_type(MSG_CHANGE_PASSWORD_RES);
    SendMsg(head, &res);
    //SendMsg(MSG_CHANGE_PASSWORD_RES, &res);
}

XAuthHandle::XAuthHandle()
{
}


XAuthHandle::~XAuthHandle()
{
}

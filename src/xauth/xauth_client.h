#ifndef XAUTH_CLIENT_H
#define XAUTH_CLIENT_H
#include "xservice_client.h"
#include <mutex>
#include <map>
#include <vector>
#define XAUTH XAuthClient::Get()
class XAuthClient:public XServiceClient
{
public:

    ~XAuthClient();
    static XAuthClient*Get()
    {
        static XAuthClient xc;
        return &xc;
    }

    bool Login(std::string username, std::string password);


    //////////////////////////////////////////////////////////////////
    ///���͵�¼����
    /// @para username �û�
    /// @para password ���루ԭ�ģ����ں����л�md5�������
    //static void LoginReq(XMsgEvent *msg_ev, std::string username, std::string password);

    //////////////////////////////////////////////////////////////////
    ///���͵�¼����
    /// @para username �û�
    /// @para password ���루ԭ�ģ����ں����л�md5�������
    void LoginReq(std::string username, std::string password);


    //���token �Ƿ���Ч ���±��ص� login��¼����
    void CheckTokenReq(std::string token) ;


    //////////////////////////////////////////////////////////////////
    ///����û���Ϣ
    void AddUserReq(const xmsg::XAddUserReq *user);

    //////////////////////////////////////////////////////////////////
    ///�޸�������Ϣ
    void ChangePasswordReq(const xmsg::XChangePasswordReq *pass);


    /////////////////////////////////////////////////////////////////
    /// �����¼
    /// @timeoue_ms ���ĳ�ʱʱ�䣬����������쳣�����̷���
    /// ���token��Ҫ���ڣ����Զ����͸�������
    /// @return ʧ�ܷ��ش��� token ��
    bool GetLoginInfo(std::string username,xmsg::XLoginRes *out_info, int timeoue_ms=200);

    xmsg::XLoginRes GetLogin();

    //////////////////////////////////////////////////////////////////
    ///ע����շ���������Ϣ����
    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_LOGIN_RES, (MsgCBFunc)&XAuthClient::LoginRes);
        RegCB(xmsg::MSG_ADD_USER_RES, (MsgCBFunc)&XAuthClient::AddUserRes);
        RegCB(xmsg::MSG_CHANGE_PASSWORD_RES, (MsgCBFunc)&XAuthClient::ChangePasswordRes);
        RegCB(xmsg::MSG_CHECK_TOKEN_RES, (MsgCBFunc)&XAuthClient::CheckTokenRes);
    }

    //��ǰ��¼���û���
    std::string cur_user_name(){ return cur_user_name_; }

private:
    //��ǰ��¼���û���
    std::string cur_user_name_;
    XAuthClient();

    //////////////////////////////////////////////////////////////////
    ///���յ�¼��Ϣ
    void LoginRes(xmsg::XMsgHead *head, XMsg *msg);

    //////////////////////////////////////////////////////////////////
    ///��������û���Ϣ
    void AddUserRes(xmsg::XMsgHead *head, XMsg *msg);

    //////////////////////////////////////////////////////////////////
    ///�����޸�������Ϣ
    void ChangePasswordRes(xmsg::XMsgHead *head, XMsg *msg);

    void CheckTokenRes(xmsg::XMsgHead *head, XMsg *msg);
    
    //��¼���ݣ�����token�� key���û��� ͬһ���û�֧�ֶ���ͻ��˵�¼
    //std::map<std::string,std::vector<xmsg::XLoginRes> > login_map_;
    std::map<std::string, xmsg::XLoginRes > login_map_;
    //xmsg::XLoginRes login_;
    std::mutex logins_mutex_;

};


#endif // !XAUTH_CLIENT_H
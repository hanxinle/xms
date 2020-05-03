#include "xauth_client.h"
#include "xtools.h"
#include <thread>
using namespace std;
using namespace xmsg;
bool XAuthClient::Login(std::string username, std::string password)
{
    LoginReq(username, password);

    auto res =  GetLogin();
    if (res.res() == XLoginRes::ERROR)
        return false;
    return true;
}

xmsg::XLoginRes XAuthClient::GetLogin()
{
    xmsg::XLoginRes res;

    if (!GetLoginInfo(cur_user_name_, &res, 3000))
    {
        res.set_res(XLoginRes::ERROR);
        return res;
    }
    return res;
}
/////////////////////////////////////////////////////////////////
/// �����¼
/// @timeoue_ms ���ĳ�ʱʱ�䣬����������쳣�����̷���
/// ���token��Ҫ���ڣ����Զ����͸�������
/// @return ʧ�ܷ��ش��� error��
//bool XAuthClient::GetToken(int timeoue_ms, std::string &token)
bool XAuthClient::GetLoginInfo(string username, xmsg::XLoginRes *out_info, int timeoue_ms)
{
    if (!out_info)return false;
    int count = timeoue_ms / 10;
    if (count <= 0) count = 1;
    int i = 0;
    for (; i < count; i++)
    {
        {
            logins_mutex_.lock();
            cout << login_map_.size() << ";" << flush;
            auto login_ptr = login_map_.find(username);
            if (login_ptr == login_map_.end())
            {
                logins_mutex_.unlock();
                this_thread::sleep_for(10ms);
                continue;
            }
            auto login = login_ptr->second;
            logins_mutex_.unlock();

            if (login.res() == XLoginRes::OK)
            {
                //token = login_.token();
                out_info->CopyFrom(login);
                return true;
            }
            return false;
        }
        //this_thread::sleep_for(10ms);
    }
    //XMutex mux(&logins_mutex_);
    //out_info->CopyFrom(login_);
    return false;
}


//���token �Ƿ���Ч ���±��ص� login��¼����
void XAuthClient::CheckTokenReq(std::string token)
{

}

void XAuthClient::CheckTokenRes(xmsg::XMsgHead *head, XMsg *msg)
{

}
//////////////////////////////////////////////////////////////////
///���͵�¼����
/// @para username �û�
/// @para password ���루ԭ�ģ����ں����л�md5�������
//void  XAuthClient::LoginReq(XMsgEvent *msg_ev, std::string username, std::string password)
//{
//    if (!msg_ev)
//    {
//        LOGERROR("XAuthClient::LoginReq failed!msg_ev is NULL");
//        return;
//    }
//    XLoginReq req;
//    req.set_username(username);
//    char md5_password[128] = { 0 };
//    XMD5_base64((unsigned char *)password.data(), password.size(), md5_password);
//    req.set_password(md5_password);
//    XMsgHead head;
//    head.set_msg_type(MSG_LOGIN_REQ);
//    head.set_service_name(AUTH_NAME);
//
//    {
//        XMutex mux(&logins_mutex_);
//
//    }
//    msg_ev->SendMsg(&head, &req);
//}
//////////////////////////////////////////////////////////////////
///���͵�¼����
void XAuthClient::LoginReq(std::string username, std::string password)
{
    cur_user_name_ = username;
    XLoginReq req;
    req.set_username(username);
    //char md5_password[128] = { 0 };
    auto md5_password = XMD5_base64((unsigned char *)password.data(), password.size());
    req.set_password(md5_password);
    //XMsgHead head;
    //head.set_msg_type(MSG_LOGIN_REQ);
    //head.set_service_name(AUTH_NAME);
    //�����ϴεĵ�¼��Ϣ
    {
        //XMutex mux(&logins_mutex_);
        XMUTEX(&logins_mutex_);
        login_map_.erase(username);// = res;
    }

    SendMsg(MSG_LOGIN_REQ, &req);
    
    //SendMsg(&head, &req);
    //SendMsg(&head, &req);
}

//////////////////////////////////////////////////////////////////
///����û���Ϣ
void XAuthClient::AddUserReq(const xmsg::XAddUserReq *user)
{
    if (!user)return;
    XAddUserReq req;
    req.CopyFrom(*user);
    string pass = user->password();
    
    auto pass_md = XMD5_base64((unsigned char*)pass.c_str(), pass.size());
    req.set_password(pass_md);
    //SendMsg(MSG_ADD_USER_REQ, &req);
    static int i = 0;
    i++;
    //XMsgHead head;
    //head.set_msg_type(MSG_ADD_USER_REQ);
    //head.set_service_name(AUTH_NAME);
    //head.set_msg_id(i);
    //{
    //    XMUTEX(&logins_mutex_);
    //    head.set_token(login_.token());
    //    head.set_username(login_.username());
    //}
    SendMsg(MSG_ADD_USER_REQ, &req);

    cout << i << "XAuthClient::Get()->AddUserReq(&req);" << endl;
}

//////////////////////////////////////////////////////////////////
///�޸�������Ϣ
void XAuthClient::ChangePasswordReq(const xmsg::XChangePasswordReq *pass)
{
     SendMsg(MSG_CHANGE_PASSWORD_REQ, pass);

    //XMsgHead head;
    //head.set_msg_type(MSG_CHANGE_PASSWORD_REQ);
    //head.set_service_name(AUTH_NAME);
    //SendMsg(&head, pass);
}

//////////////////////////////////////////////////////////////////
///���յ�¼��Ϣ
void XAuthClient::LoginRes(xmsg::XMsgHead *head, XMsg *msg)
{
    static int count;
    count++;
    cout << "LoginRes"<< count << flush;
    XLoginRes res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("XLoginRes ParseFromArray failed!");
        return;
    }

    LOGINFO(res.DebugString());
    LOGINFO(head->DebugString());
    {
        cout << "begin XMUTEX(&logins_mutex_)" << endl;
        XMUTEX(&logins_mutex_);
        cout << "end XMUTEX(&logins_mutex_)" << endl;
        if (!res.username().empty())
        {
            login_map_[res.username()] = res;
            set_login(&res);
            //auto login_list = login_map_.find(res.username());
            //if (login_list == login_map_.end())
            //{
            //    login_map_[res.username()];
            //}
            //login_list = login_map_.find(res.username());
   
            //auto ptr = login_list->second.begin();
            //for (; ptr!= login_list->second.end(); ptr++)
            //{
            //   // if(ptr->expired_time() > )
            //    login_list->second.erase(ptr);
            //}
            //int size = login_list->second.size();
            ////ɾ�����ڵ�
            //for (int i = 0; i < size; i++)
            //{
            //    login_list->second.erase(i);
            //}
            //login_map_[res.username()].push_back(res);

        }
            
    }

}
//////////////////////////////////////////////////////////////////
///��������û���Ϣ
void XAuthClient::AddUserRes(xmsg::XMsgHead *head, XMsg *msg)
{
    XMessageRes res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("AddUser failed!ParseFromArray failed!");
        return;
    }
    if (res.return_() == XMessageRes::ERROR)
    {
        LOGDEBUG(res.msg().c_str());
        LOGDEBUG("AddUser failed!");
        return;
    }
    LOGDEBUG("AddUser success!");
    return;
}

//////////////////////////////////////////////////////////////////
///�����޸�������Ϣ
void XAuthClient::ChangePasswordRes(xmsg::XMsgHead *head, XMsg *msg)
{
    XMessageRes res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("ChangePassword failed!ParseFromArray failed!");
        return;
    }
    if (res.return_() == XMessageRes::ERROR)
    {
        LOGDEBUG(res.msg().c_str());
        LOGDEBUG("ChangePasswordRes failed!");
        return;
    }
    LOGDEBUG("ChangePasswordRes success!");
    return;
}
XAuthClient::XAuthClient()
{
    RegMsgCallback();
    set_service_name(AUTH_NAME);
}


XAuthClient::~XAuthClient()
{
}

#ifndef XAUTH_DAO_H
#define XAUTH_DAO_H

#include <string>
namespace LX {
    class  LXMysql;
}
namespace xmsg {
    class  XAddUserReq;
    class XChangePasswordReq;
    class XLoginReq;
    class XLoginRes;
    class XMsgHead;

}
class XAuthDao
{
public:
    static XAuthDao *Get()
    {
        static XAuthDao dao;
        return &dao;
    }
    ///��ʼ�����ݿ�
    //bool Init(const char *ip, const char *user, const char*pass, const char*db_name, int port = 3306);
    bool Init();

    ///��װ���ݿ�ı�
    bool Install();

    ///////////////////////////////////////////////////////////////////////////////////
    /// ��¼ϵͳ
    /// @para username �û���
    /// @hash_password ����md5 hash����������
    /// @token ����ɹ����ص�¼���� ����������ش���ԭ��
    /// @timeout_sec ���Ƶĳ�ʱʱ�䣨�룩��Ĭ��30����
    /// @return ��¼�ɹ�����ʧ��
    //bool Login(std::string username, std::string hash_password,std::string &token,int timeout_sec=1200);
    bool Login(const xmsg::XLoginReq *user_req,xmsg::XLoginRes *user_res, int timeout_sec);

    bool CheckToken(const xmsg::XMsgHead *head, xmsg::XLoginRes *user_res);


    //////////////////////////////////////////////////////////////////
    ///��������û���Ϣ
    bool AddUser(const xmsg::XAddUserReq *user);

    //////////////////////////////////////////////////////////////////
    ///�����޸�������Ϣ
    bool ChangePassword(const xmsg::XChangePasswordReq *pass);



    XAuthDao();
    ~XAuthDao();
private:
    LX::LXMysql *my_ = 0;
};


#endif

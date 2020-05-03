#pragma once

namespace LX {
    class  LXMysql;
}
namespace xmsg {
    class  XAddLogReq;
}
class XLogDAO
{
public:
    ~XLogDAO();
    static XLogDAO *Get()
    {
        static XLogDAO xd;
        return &xd;
    }
    bool Init();

    ///��װ���ݿ�ı�
    bool Install();

    bool AddLog(const xmsg::XAddLogReq *req);
private:
    //mysql���ݿ��������
    LX::LXMysql *my_ = 0;

    XLogDAO();
};


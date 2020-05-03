#pragma once

#include "xmsg_com.pb.h"
namespace LX {
    class  LXMysql;
}
class ConfigDao
{
public:
    static ConfigDao *Get()
    {
        static ConfigDao dao;
        return &dao;
    }
    ///��ʼ�����ݿ�
    //bool Init(const char *ip, const char *user, const char*pass, const char*db_name, int port = 3306);
    bool Init();

    ///��װ���ݿ�ı�
    bool Install();

    ///�������ã�������У��͸���
    bool SaveConfig(const xmsg::XConfig *conf);

    //��ȡ����
    xmsg::XConfig LoadConfig(const char *ip, int port);

    ///////////////////////////////////////////////////////
    ///��ȡ��ҳ�������б�
    ///@para page �� 1��ʼ
    ///@para page_count ÿҳ����
    xmsg::XConfigList LoadAllConfig(int page, int page_count);

    //ɾ��ָ��������
    bool DeleteConfig(const char *ip, int port);

    virtual ~ConfigDao();
private:
    //mysql���ݿ��������
    LX::LXMysql *my_ = 0;
    ConfigDao();
};


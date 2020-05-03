#include "xconfig_manager.h"
#include "xtools.h"
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <map>
#include <thread>
#include <fstream>
using namespace std;
using namespace google;
using namespace protobuf;
using namespace compiler;
using namespace xmsg;
//�洢��ȡ�������б�
static XConfigList *all_config = 0;
static mutex all_config_mutex;
//����ɾ����������
void XConfigManager::DeleteConfig(const char *ip, int port)
{
    if (!ip || strlen(ip) == 0 || port < 0 || port>65535)
    {
        LOGDEBUG("DeleteConfig failed!port or ip error");
        return;
    }
    XLoadConfigReq req;
    req.set_service_ip(ip);
    req.set_service_port(port);
    //������Ϣ�������
    SendMsg(MSG_DEL_CONFIG_REQ, &req);
}

//ɾ��������Ӧ
void XConfigManager::DeleteConfigRes(xmsg::XMsgHead *head, XMsg *msg)
{
    XMessageRes res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("ParseFromArray failed!");
        return;
    }
    if (res.return_() == XMessageRes::OK)
    {
        LOGDEBUG("ɾ�����óɹ�!");
        return;
    }
    LOGDEBUG("ɾ������ʧ��");
}
void XConfigManager::set_login(xmsg::XLoginRes login)
{
    XMutex mux(&login_mutex_);
    this->login_ = login;

}

bool XConfigManager::SendMsg(xmsg::MsgType type, const google::protobuf::Message *message)
{
    XMsgHead head;
    head.set_msg_type(type);
    if (!login_.token().empty())
    {
        XMutex mux(&login_mutex_);
        head.set_token(login_.token());
        head.set_username(login_.username());
        head.set_rolename(login_.rolename());
    }

    head.set_service_name(CONFIG_NAME);
    return XMsgEvent::SendMsg(&head, message);
}

///��Ӧ��ȡ�����б�  �洢������
void XConfigManager::LoadAllConfigRes(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("��Ӧ��ȡ�����б� ");
    XMutex mux(&all_config_mutex);
    if (!all_config)
        all_config = new XConfigList();
    all_config->ParseFromArray(msg->data, msg->size);
}

/////////////////////////////////////////////////////////////////////
/// ��ȡȫ�������б�
/// 1 �Ͽ������Զ�����
/// 2 �ȴ��������
xmsg::XConfigList XConfigManager::GetAllConfig(int page, int page_count, int timeout_sec)
{
    //������ʷ����
    {
        XMutex mux(&all_config_mutex);
        delete all_config;
        all_config = NULL;
    }
    XConfigList confs;
    //1 �Ͽ������Զ�����
    if (!AutoConnect(timeout_sec))
        return confs;

    //2 ���ͻ�ȡ�����б����Ϣ
    XLoadAllConfigReq req;
    req.set_page(page);
    req.set_page_count(page_count);
    SendMsg(MSG_LOAD_ALL_CONFIG_REQ, &req);

    //10�������һ��
    int count = timeout_sec * 100;
    for (int i = 0; i < count; i++)
    {
        {
            //����return ֮������ͷ�
            XMutex mux(&all_config_mutex);
            if (all_config)
            {
                return *all_config;
            }
        }
        //�Ƿ��յ���Ӧ
        this_thread::sleep_for(10ms);
    }

    return confs;
}


//���յ��������õ���Ϣ
void XConfigManager::SendConfigRes(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("���յ��ϴ����õķ���");
    XMessageRes res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("ParseFromArray failed!");
        if (SendConfigResCB)
            SendConfigResCB(false, "ParseFromArray failed!");

        return;
    }
    if (res.return_() == XMessageRes::OK)
    {
        LOGDEBUG("�ϴ����óɹ�!");
        if (SendConfigResCB)
            SendConfigResCB(true, "�ϴ����óɹ�!");
        return;
    }
    stringstream ss;
    ss << "�ϴ�����ʧ��:" << res.msg();
    if (SendConfigResCB)
        SendConfigResCB(false, ss.str().c_str());
    LOGDEBUG(ss.str().c_str());

}
//��������
void XConfigManager::SendConfig(xmsg::XConfig *conf)
{
    LOGDEBUG("��������");
    SendMsg(MSG_SAVE_CONFIG_REQ, conf);
}


//��ȡ�������� IP���ΪNULL ��ȡ�����������ĵĵ�ַ
void XConfigManager::LoadConfig(const char *ip, int port)
{
    LOGDEBUG("��ȡ��������");
    if (port < 0 || port>65535)
    {
        LOGDEBUG("LoadConfig failed!port error");
        return;
    }
    XLoadConfigReq req;
    if (ip) //IP���ΪNULL ��ȡ�����������ĵĵ�ַ
        req.set_service_ip(ip);
    req.set_service_port(port);
    //config_.Clear();
    //config_.service_ip().empty();
    //������Ϣ�������
    SendMsg(MSG_LOAD_CONFIG_REQ, &req);
}

///��Ӧ��ȡ�Լ�������  �洢������
void XConfigManager::LoadConfigRes(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("��ȡ������Ӧ");
    //XMutex mux(&config_mutex_);
    XConfig config;
    if (!config.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("LoadConfigRes conf.ParseFromArray failed!");
        return;
    }
    LOGDEBUG(config.DebugString().c_str());
    if (LoadConfigResCB)
    {
        LoadConfigResCB(config);
    }
    ////key ip_port
    //stringstream key;
    //key << conf.service_ip() << "_" << conf.service_port();
    ////��������
    //config_mutex_.lock();
    //conf_map[key.str()] = conf;
    //config_mutex_.unlock();

    ////û�б�������
    //if (local_port_ <= 0 || !cur_service_conf)
    //    return;
    //stringstream local_key;
    //string ip = local_ip_;
    //if (ip.empty())
    //{
    //    ip = conf.service_ip();
    //}
    //local_key << ip << "_" << local_port_;
    //if (key.str() != local_key.str())
    //{
    //    return;
    //}
    //XMutex mux(&cur_service_conf_mutex);
    //if (!cur_service_conf && !cur_service_conf->ParseFromString(conf.private_pb()))
    //{
    //    return;
    //}
    //LOGDEBUG(cur_service_conf->DebugString().c_str());
    ////�洢�������ļ�
    //�ļ��� [port]_conf.cache  20030_conf.cache
    //stringstream ss;
    //ss << local_port_ << "_conf.cache";
    //ofstream ofs;
    //ofs.open(ss.str(), ios::binary);
    //if (!ofs.is_open())
    //{
    //    LOGDEBUG("save local config failed!");
    //    return;
    //}
    //cur_service_conf->SerializePartialToOstream(&ofs);
    //ofs.close();
}


XConfigManager::XConfigManager()
{
    RegMsgCallback();
}


XConfigManager::~XConfigManager()
{
}

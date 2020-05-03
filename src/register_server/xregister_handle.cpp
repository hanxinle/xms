#include "xregister_handle.h"
#include "xtools.h"
#include "xmsg_com.pb.h"
#include "xlog_client.h"
using namespace xmsg;
using namespace std;

///ע������б�Ļ���
static XServiceMap * service_map = 0;

//���̷߳��ʵ���
static mutex service_map_mutex;

//���շ���ķ�������
void XRegisterHandle::GetServiceReq(xmsg::XMsgHead *head, XMsg *msg)
{
    //��ʱֻ����ȫ��
    LOGDEBUG("���շ���ķ�������");
    XGetServiceReq req;

    //������
    xmsg::XServiceMap res;
    res.mutable_res()->set_return_(XMessageRes_XReturn_ERROR);

    if (!req.ParseFromArray(msg->data, msg->size))
    {
        stringstream ss;
        ss << "req.ParseFromArray failed!";
        LOGINFO(ss.str().c_str());
        res.mutable_res()->set_msg(ss.str().c_str());
        SendMsg(MSG_GET_SERVICE_RES, &res);
        return;
    }

    string service_name = req.name();
    stringstream ss;
    ss << "GetServiceReq : service name " << service_name;
    LOGDEBUG(ss.str().c_str());
    XServiceMap *send_map = &res;

    //����ȫ��΢��������
    service_map_mutex.lock();

    if (!service_map)
        service_map = new XServiceMap();
    //LOGINFO("service_map=" + service_map->DebugString());
    //����ȫ��
    if (req.type() == XServiceType::ALL)
    {
        send_map = service_map;
        send_map->set_type(XServiceType::ALL);
    }
    else //���ص���
    {
        auto smap = service_map->mutable_service_map();
        if (smap && smap->find(service_name) != smap->end())
        {
            (*send_map->mutable_service_map())[service_name] = (*smap)[service_name];
        }
    }
    service_map_mutex.unlock();

    LOGDEBUG(send_map->DebugString());
    //LOGINFO("service_map2=" + service_map->DebugString());
    //LOGINFO("service_name=" + service_name);

    //LOGINFO("send_map=" + send_map->DebugString());

    //���ص��ֻ���ȫ��
    send_map->set_type(req.type());
    send_map->mutable_res()->set_return_(XMessageRes_XReturn_OK);
    SendMsg(MSG_GET_SERVICE_RES, send_map);
}

//���շ����ע������
void XRegisterHandle::RegisterReq(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("����˽��յ��û���ע������");

    //��Ӧ����Ϣ
    XMessageRes res;
    
    //��������
    XServiceInfo req;
    if (!req.ParseFromArray(msg->data, msg->size))
    {
        LOGERROR("XRegisterReq ParseFromArray failed!");
        res.set_return_(XMessageRes::ERROR);
        res.set_msg("XRegisterReq ParseFromArray failed!");
        SendMsg(MSG_REGISTER_RES, &res);
        return;
    }
    
    //���յ��û��ķ������ơ�����IP������˿�
    string service_name = req.name();
    if (service_name.empty())
    {
        string error = "service_name is empty!";
        LOGERROR(error.c_str());
        res.set_return_(XMessageRes::ERROR);
        res.set_msg(error);
        SendMsg(MSG_REGISTER_RES, &res);
        return;
    }

    string service_ip = req.ip();
    if (service_ip.empty())
    {
        LOGERROR("service_ip is empty : client ip");
        service_ip = this->client_ip();
    }

    int service_port = req.port();
    if (service_port <= 0 || service_port > 65535)
    {
        stringstream ss;
        //string error = "service_port is error!";
        ss << "service_port is error!" << service_port;
        LOGERROR(ss.str().c_str());
        res.set_return_(XMessageRes::ERROR);
        res.set_msg(ss.str());
        SendMsg(MSG_REGISTER_RES, &res);
        return;
    }
    
    //�����û�ע����Ϣ����
    stringstream ss;
    ss << "���յ��û�ע����Ϣ:" << service_name << "|" << service_ip << ":" << service_port;
    LOGINFO(ss.str().c_str());

    //�洢�û�ע����Ϣ������Ѿ�ע����Ҫ����
    {
        XMutex mutex(&service_map_mutex);
        if (!service_map)
            service_map = new XServiceMap();
        //map��ָ��
        auto smap = service_map->mutable_service_map();

        //�Ƿ���ͬ�����Ѿ�ע��
        //��Ⱥ΢����
        auto service_list = smap->find(service_name);
        if (service_list == smap->end())
        {
            //û��ע���
            (*smap)[service_name] = XServiceList();
            service_list = smap->find(service_name);
        }
        auto services = service_list->second.mutable_service();
        //�����Ƿ���ͬip�Ͷ˿ڵ�
        for (auto service : (*services))
        {
            if (service.ip() == service_ip && service.port() == service_port)
            {
                stringstream ss;
                ss <<service_name<<"|"<< service_ip << ":"
                    << service_port << "΢�����Ѿ�ע���";
                LOGERROR(ss.str().c_str());
                res.set_return_(XMessageRes::ERROR);
                res.set_msg(ss.str());
                SendMsg(MSG_REGISTER_RES, &res);
                return;
            }
        }
        //����µ�΢����
        auto ser  = service_list->second.add_service();
        ser->set_ip(service_ip);
        ser->set_port(service_port);
        ser->set_name(service_name);
        ser->set_is_find(req.is_find());
        stringstream ss;
        ss << service_name << "|" << service_ip << ":"
            << service_port << "�µ�΢����ע��ɹ���";
        LOGERROR(ss.str().c_str());
    }

    res.set_return_(XMessageRes::OK);
    res.set_msg("OK");
    SendMsg(MSG_REGISTER_RES, &res);
}

XRegisterHandle::XRegisterHandle()
{
}


XRegisterHandle::~XRegisterHandle()
{
}

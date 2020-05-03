#include "xregister_client.h"
#include "xlog_client.h"
#include "xmsg_com.pb.h"
#include "xtools.h"
#include <thread>
#include <fstream>
using namespace xmsg;

using namespace std;

///ע������б�Ļ���
static XServiceMap * service_map = 0;
static XServiceMap * client_map = 0;

//���̷߳��ʵ���
static mutex service_map_mutex;


/////////////////////////////////////////////////////////////
///�����л�ȡ΢�����б������
///@para service_name == NULL ��ȡȫ��
void XRegisterClient::GetServiceReq(const char *service_name)
{
    LOGDEBUG("GetServiceReq");
    XGetServiceReq req;
    if (service_name)
    {
        req.set_type(XServiceType::ONE);
        req.set_name(service_name);
    }
    else
    {
        req.set_type(XServiceType::ALL);
    }

    SendMsg(MSG_GET_SERVICE_REQ, &req);
}

//��ȡ���ػ���
bool XRegisterClient::LoadLocalFile()
{
    if (!service_map)
    {
        service_map = new XServiceMap();
    }
    LOGDEBUG("Load local register data");
    stringstream ss;
    ss << "register_" << service_name_ << service_ip_ << service_port_ << ".cache";
    ifstream ifs;
    ifs.open(ss.str(), ios::binary);
    if (!ifs.is_open())
    {
        stringstream log;
        log << "Load local register data failed!";
        log << ss.str();
        LOGDEBUG(log.str().c_str());
        return false;
    }
    service_map->ParseFromIstream(&ifs);
    ifs.close();
    return true;

}

/////////////////////////////////////////////////////////////////////////////
/// ��ȡָ���������Ƶ�΢�����б� ������������
/// 1 �ȴ����ӳɹ� 2 ���ͻ�ȡ΢�������Ϣ 3 �ȴ�΢�����б���Ϣ�������п����õ���һ�ε����ã�
/// @para service_name ��������
/// @para timeout_sec ��ʱʱ��
/// @return �����б�
xmsg::XServiceList XRegisterClient::GetServcies(const char *service_name, int timeout_sec)
{
    xmsg::XServiceList re;
    //ʮ�����ж�һ��
    int totoal_count = timeout_sec * 100;
    int count = 0;
    //1 �ȴ����ӳɹ�
    while (count < totoal_count)
    {
        //cout << "@" << flush;
        if (is_connected())
            break;
        this_thread::sleep_for(chrono::milliseconds(10));
        count++;
    }
    if (!is_connected())
    {
        LOGDEBUG("���ӵȴ���ʱ");
        XMutex mutex(&service_map_mutex);
        //ֻ�е�һ�ζ�ȡ����
        if (!service_map)
        {
            LoadLocalFile();
        }
        return re;
    }

    //2 ���ͻ�ȡ΢�������Ϣ
    GetServiceReq(service_name);
    
    //3 �ȴ�΢�����б���Ϣ�������п����õ���һ�ε����ã�
    while (count < totoal_count)
    {
        cout << "." << flush;
        XMutex mutex(&service_map_mutex);
        if (!service_map)
        {
            this_thread::sleep_for(chrono::milliseconds(10));
            count++;
            continue;
        }
        auto m = service_map->mutable_service_map();
        if (!m)
        {
            //cout << "#" << flush;
            //û���ҵ�ָ����΢����
            GetServiceReq(service_name);
            this_thread::sleep_for(chrono::milliseconds(100));
            count+=10;
            continue;
        }
        auto s = m->find(service_name);
        if (s == m->end())
        {
           // cout << "+" << flush;
            //û���ҵ�ָ����΢����
            GetServiceReq(service_name);
            this_thread::sleep_for(chrono::milliseconds(100));
            count += 10;
            continue;
        }
        re.CopyFrom(s->second);
        return re;
    }
    return re;

    //XMutex mutex(&service_map_mutex);
}

xmsg::XServiceMap *XRegisterClient::GetAllService()
{
    XMutex mutex(&service_map_mutex);
    //LoadLocalFile();
    if (!service_map)
    {
        return NULL;
    }
    if (!client_map)
    {
        client_map = new XServiceMap();
    }
    client_map->CopyFrom(*service_map);
    return client_map;
}

//��ȡ�����б����Ӧ
void XRegisterClient::GetServiceRes(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("GetServiceRes");

    XMutex mutex(&service_map_mutex);
    //�Ƿ��滻ȫ������
    bool is_all = false;
    XServiceMap *cache_map;
    XServiceMap tmp;
    cache_map = &tmp;
    if (!service_map)
    {
        service_map = new XServiceMap();
        cache_map = service_map;
        is_all = true;
    }
    if (!cache_map->ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("service_map.ParseFromArray failed!");
        return;
    }
    
    if(cache_map->type() == XServiceType::ALL)
    {
        is_all = true;
    }

    ///////////////////////////////////////////////////////////
    //�ڴ滺��ˢ��
    if (cache_map == service_map)
    {
        //�洢�����Ѿ�ˢ��
    }
    else
    {
        if (is_all)
        {
            service_map->CopyFrom(*cache_map);
        }
        else
        {
            /// ���ն�ȡ��cmap���ݴ���  service_map �ڴ滺��
            auto cmap = cache_map->mutable_service_map();

            //ȡ��һ��
            if (!cmap || cmap->empty())return;
            auto one = cmap->begin();

            auto smap = service_map->mutable_service_map();
            //�޸Ļ���
            (*smap)[one->first] = one->second;
        }
    }


    ///////////////////////////////////////////////////////////
    //���̻���ˢ�� ����Ҫ����ˢ��Ƶ��
    stringstream ss;
    ss << "register_" << service_name_ << service_ip_ << service_port_ << ".cache";
    LOGDEBUG("Save local file!");
    if (!service_map)return;

    ofstream ofs;
    ofs.open(ss.str(), ios::binary);
    if (!ofs.is_open())
    {
        LOGDEBUG("save local file faield!");
        return;
    }
    //����Ҫ�趨��Ч��
    service_map->SerializePartialToOstream(&ofs);
    ofs.close();

    //LOGDEBUG(service_map->DebugString());
    //�����ǻ�ȡһ�ֻ���ȫ�� ˢ�»���
    //һ�� ֻˢ�´���΢�����б�������
    
    //ȫ�� ˢ�����л�������
}

//���շ����ע����Ӧ
void XRegisterClient::RegisterRes(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("RegisterRes");
    XMessageRes res;
    if (!res.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("XRegisterClient::RegisterRes failed!res.ParseFromArray failed!");
        return;
    }
    if (res.return_() == XMessageRes::OK)
    {
        LOGINFO("RegisterRes success");
        return;
    }
    stringstream ss;
    ss << "RegisterRes failed! " << res.msg();
    LOGINFO(ss.str().c_str());
}

void XRegisterClient::ConnectedCB()
{
    //����ע����Ϣ
    LOGDEBUG("connected start send MSG_REGISTER_REQ ");
    XServiceInfo req;
    req.set_name(service_name_);
    req.set_ip(service_ip_);
    req.set_port(service_port_);
    req.set_is_find(is_find_);
    SendMsg(MSG_REGISTER_REQ, &req);
}
//��ʱ�������ڷ�������
void XRegisterClient::TimerCB()
{
    static long long count = 0;
    count++;
    XMsgHeart req;
    req.set_count(count);
    SendMsg(MSG_HEART_REQ, &req);
}

///////////////////////////////////////////////////////////////////////
//// ��ע������ע����� 
/// @para service_name ΢��������
/// @para port ΢����ӿ�
/// @para ip ΢�����ip���������NULL������ÿͻ������ӵ�ַ
void XRegisterClient::RegisterServer(const char *service_name, int port, const char *ip, bool is_find)
{
    is_find_ = is_find;
    //ע����Ϣ�ص�����
    RegMsgCallback();
    //������Ϣ��������
    //�����������Ƿ�ɹ���
    ///ע�����ĵ�IP��ע�����ĵĶ˿�
    //_CRT_SECURE_NO_WARNINGS
    if (service_name)
        strcpy(service_name_, service_name);
    if (ip)
        strcpy(service_ip_, ip);
    service_port_ = port;

    //�����Զ�����
    set_auto_connect(true);

    //�趨������ʱ��
    set_timer_ms(3000);

    //���Ĭ�ϵ�IP�Ͷ˿�
    if (server_ip()[0] == '\0')
    {
        set_server_ip("127.0.0.1");
    }
    if (server_port() <= 0)
    {
        set_server_port(REGISTER_PORT);
    }

    //��������뵽�̳߳���
    StartConnect();

    LoadLocalFile();
}

XRegisterClient::XRegisterClient()
{
}


XRegisterClient::~XRegisterClient()
{
}

#include "xservice_proxy.h"
#include "xmsg_com.pb.h"
#include "xtools.h"
#include "xlog_client.h"
#include "xregister_client.h"
#include <thread>
using namespace std;
using namespace xmsg;
///��ʼ��΢�����б�ע�����Ļ�ȡ������������
bool XServiceProxy::Init()
{
    //1 ��ע�����Ļ�ȡ΢�����б�
    //��������
    //XServiceMap service_map;
    //auto smap = service_map.mutable_service_map();
    //XServiceMap::XServiceList list;
    //{
    //auto service = list.add_service();
    //service->set_ip("127.0.0.1");
    //service->set_port(20011);
    //service->set_name("dir");
    //}
    //{
    //    auto service = list.add_service();
    //    service->set_ip("127.0.0.1");
    //    service->set_port(20012);
    //    service->set_name("dir");
    //}
    //{
    //    auto service = list.add_service();
    //    service->set_ip("127.0.0.1");
    //    service->set_port(20013);
    //    service->set_name("dir");
    //}
    //(*smap)["dir"] = list;
    //cout << service_map.DebugString() << endl;

    //// ��΢����������
    ////����XServiceMap����
    //for (auto m : (*smap))
    //{
    //    client_map_[m.first] = vector<XServiceProxyClient*>();
    //    for (auto s : m.second.service())
    //    {
    //        auto proxy = new XServiceProxyClient();
    //        proxy->set_server_ip(s.ip().c_str());
    //        proxy->set_server_port(s.port());
    //        proxy->StartConnect();
    //        client_map_[m.first].push_back(proxy);
    //        client_map_last_index_[m.first] = 0;
    //    }
    //}
    return true;
}

//������Ϣ�ص�
void XServiceProxy::DelEvent(XMsgEvent *ev)
{
    if (!ev)return;
    XMutex mux(&callbacks_mutex_);
    auto call = callbacks_.find(ev);
    if (call == callbacks_.end())
    {
        LOGDEBUG("callbacks_ not find!");
        return;
    }
    call->second->DelEvent(ev);
}
//���ؾ����ҵ��ͻ������ӣ��������ݷ���
bool XServiceProxy::SendMsg(xmsg::XMsgHead *head, XMsg *msg,XMsgEvent *ev)
{
    if (!head || !msg)return false;
    string service_name = head->service_name();
    
    //MSG_GET_SERVICE //��ȡ΢����ֻ�ܻ�ȡis_find=true��΢����
    if (head->msg_type() == MSG_GET_OUT_SERVICE_REQ)
    {   
        //1 ���ؾ����ҵ��ͻ�������
        XServiceList services;
        services.set_name(service_name);
        auto client_list = client_map_.find(service_name);
        if (client_list == client_map_.end())
        {
            return ev->SendMsg(head, &services);
        }
        //�ҵ�is_find = true��; �Ϳ������ӵ�
        for (auto c : client_list->second)
        {
            if (!c->is_find() || !c->is_connected())
                continue;
            auto ser = services.add_service();
            ser->set_ip(c->server_ip());
            ser->set_port(c->server_port());
        }
        head->set_msg_type(MSG_GET_OUT_SERVICE_RES);
        return ev->SendMsg(head, &services);
    }



    XMutex mux(&client_map_mutex_);

    //1 ���ؾ����ҵ��ͻ�������
    auto client_list = client_map_.find(service_name);
    if (client_list == client_map_.end())
    {
        stringstream ss;
        ss << service_name << " client_map_ not find!";
        LOGDEBUG(ss.str().c_str());
        return false;
    }

    // ��ѯ�ҵ����õ�΢��������
    int cur_index = client_map_last_index_[service_name];
    int list_size = client_list->second.size();
    for (int i = 0; i < list_size; i++)
    {
        cur_index++;
        cur_index = cur_index % list_size;
        client_map_last_index_[service_name] = cur_index;
        auto client = client_list->second[cur_index];
        if (client->is_connected())
        {
            //�����˳�����
            XMutex mux(&callbacks_mutex_);
            callbacks_[ev] = client;

            //ת����Ϣ
            return client->SendMsg(head, msg, ev);
        }
    }
    LOGDEBUG("can't find proxy");
    return false;
}


//�����Զ��������߳�
void XServiceProxy::Start()
{
    thread th(&XServiceProxy::Main, this);
    th.detach();
}

//ֹͣ�߳�
void XServiceProxy::Stop()
{

}

void XServiceProxy::Main()
{
    //�Զ�����
    while (!is_exit_)
    {

        // ��ע�����Ļ�ȡ ΢������б����
        //��������ע������
        XRegisterClient::Get()->GetServiceReq(0);
        this_thread::sleep_for(200ms);
        auto service_map = XRegisterClient::Get()->GetAllService();
        if (!service_map)
        {
            LOGDEBUG("GetAllService : service_map is NULL");
            this_thread::sleep_for(1s);
            continue;
        }
        auto smap = service_map->service_map();
        if (smap.empty())
        {
            LOGDEBUG("XServiceProxy : service_map->service_map is NULL");
            this_thread::sleep_for(1s);
            continue;
        }

        //LOGINFO("\n=============================\n"+service_map->DebugString());
        //�������е�΢���������б�
        for (auto m : smap)
        {
            //��������΢����
            for (auto s : m.second.service())
            {
                string service_name = m.first;

                //�������Լ�
                if (service_name == API_GATEWAY_NAME)
                {
                    continue;
                }

                //��΢�����Ƿ��Ѿ�����
                XMutex mux(&client_map_mutex_);
                //��һ��΢���񣬴������󣬿�������
                if (client_map_.find(service_name) == client_map_.end())
                {
                    client_map_[service_name] = std::vector < XServiceProxyClient *>();
                }

                //�б����Ƿ����д�΢����
                bool isfind = false;
                for (auto c : client_map_[service_name])
                {
                    if (s.ip() == c->server_ip() && s.port() == c->server_port())
                    {
                        isfind = true;
                        break;
                    }
                }
                if (isfind)
                    continue;

                //�������ʹ�����ͬ��proxy
                auto proxy = XServiceProxyClient::Create(service_name);// new XServiceProxyClient();
                proxy->set_is_find(s.is_find());
                proxy->set_server_ip(s.ip().c_str());
                proxy->set_server_port(s.port());
                //���ùرպ�����Զ�����
                proxy->set_auto_delete(false);

                //������������̳߳�
                proxy->StartConnect();
                client_map_[service_name].push_back(proxy);
                client_map_last_index_[service_name] = 0;
            }
        }

        // ��ʱȫ�����»�ȡ
        for (auto m : client_map_)
        {
            for (auto c : m.second)
            {
                if (c->is_connected())
                    continue;
                if (!c->is_connecting())
                {
                    LOGDEBUG("start conncet service");
                    c->Connect();
                }
            }
        }
        this_thread::sleep_for(3000ms);
    }
}
XServiceProxy::XServiceProxy()
{
}


XServiceProxy::~XServiceProxy()
{
}

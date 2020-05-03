#include <iostream>
#include "xservice.h"
#include "xrouter_server.h"
#include "xservice_proxy.h"
#include "xregister_client.h"
#include "xmsg_com.pb.h"
#include "xconfig_client.h"
#include "xauth_proxy.h"
#include "xtools.h"
using namespace std;
using namespace xmsg;

#define REG  XRegisterClient::Get()
#define CONF XConfigClient::Get()
void ConfTimer()
{
    static string conf_ip = "";
    static int conf_port = 0;
    /////////////////////////////////////////////////////////////////
    //��ȡ������
    //cout << "config root = " << CONF->GetString("root") << endl;

    if (conf_port <= 0)
    {
        //��ע�����Ļ�ȡ�������ĵ�IP
        auto confs = REG->GetServcies(CONFIG_NAME, 1);
        cout << confs.DebugString();
        if (confs.service_size() <= 0)
            return;
        auto conf = confs.service()[0];

        if (conf.ip().empty() || conf.port() <= 0)
            return;
        conf_ip = conf.ip();
        conf_port = conf.port();
        CONF->set_server_ip(conf_ip.c_str());
        CONF->set_server_port(conf_port);
        CONF->Connect();
    }
}
int main(int argc, char *argv[])
{
    /// xms_gateway
    cout << "xms_gateway API_GATEWAY_PORT REGISTER_IP REGISTER_PORT" << endl;
    int server_port = API_GATEWAY_PORT;
    if (argc > 1)
        server_port = atoi(argv[1]);
    cout << "server port is " << server_port << endl;
    string register_ip = "";

    register_ip = XGetHostByName(API_REGISTER_SERVER_NAME);
    if (argc > 2)
        register_ip = argv[2];
    int register_port = REGISTER_PORT;
    if(argc>3)
        register_port = atoi(argv[3]);
    if(register_ip.empty())
        register_ip =  "127.0.0.1";

    XAuthProxy::InitAuth();

    //����ע�����ĵ�IP�Ͷ˿�
    XRegisterClient::Get()->set_server_ip(register_ip.c_str());
    XRegisterClient::Get()->set_server_port(register_port);

    //ע�ᵽע������
    XRegisterClient::Get()->RegisterServer(API_GATEWAY_NAME, server_port, 0,true);
    //
    ////�ȴ�ע����������
    XRegisterClient::Get()->WaitConnected(3);
    /*for (;;)
    {
        XRegisterClient::Get()->GetServiceReq(0);
        auto tmp = XRegisterClient::Get()->GetAllService();
        if (tmp)
        {
            cout << tmp->DebugString();
        }
        this_thread::sleep_for(100ms);
    }*/
    XRegisterClient::Get()->GetServiceReq(0);

    ////΢����������ӳ�ʼ��
    XServiceProxy::Get()->Init();

    ////�����Զ�����
    XServiceProxy::Get()->Start();

    static XGatewayConfig cur_conf;
    if (XConfigClient::Get()->StartGetConf(0, server_port, &cur_conf,ConfTimer))
        cout << "��ʼ���������ĳɹ�" << cur_conf.DebugString() << endl;
    //�����������ģ���ȡ·������
    //�ȴ����û�ȡ�ɹ�
    //auto confs = XRegisterClient::Get()->GetServcies(CONFIG_NAME, 10);
    //cout << "=================================================" << endl;
    //cout << confs.DebugString() << endl;
    ////��������IP��ȡʧ�ܣ���ȡ��������
    //if (confs.service_size() <= 0)
    //{
    //    cout << "find config service failed!" << endl;
    //}
    //else
    //{
    //    //ֻȡ��һ����������
    //    auto conf = confs.service()[0];
        //static XGatewayConfig cur_conf;
        //if (XConfigClient::Get()->StartGetConf(
        //    conf.ip().c_str(), conf.port()
        //    , 0, server_port, &cur_conf))
        //    cout << "�����������ĳɹ�" << cur_conf.DebugString() << endl;
    //}


    //����·�ɷ���
    XRouterServer service;
    service.set_server_port(server_port);
    service.Start();
    XThreadPool::Wait();
    return 0;
}
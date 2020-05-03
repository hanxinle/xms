#include <iostream>
#include "xconfig_client.h"
#include "xregister_client.h"
#include "xmsg_com.pb.h"
#include "xconfig_manager.h"
#include <thread>
using namespace std;
using namespace xmsg;
#define REG  XRegisterClient::Get()
#define CONF XConfigClient::Get()
#define MCONF XConfigManager::Get()
void ConfTimer()
{
    static string conf_ip = "";
    static int conf_port = 0;
    /////////////////////////////////////////////////////////////////
    //��ȡ������
    cout << "config root = " << CONF->GetString("root") << endl;

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
int main(int argc,char *argv[])
{
    int client_port = 4000;
    
    //����ע�����ĵ�IP�Ͷ˿�
    REG->set_server_ip("127.0.0.1");
    REG->set_server_port(REGISTER_PORT);
    REG->RegisterServer("test_config", client_port, 0);
    //this_thread::sleep_for(2000ms);
    
    
   
    //��ʼ����������
    XDirConfig tmp_conf;
    CONF->StartGetConf(0, client_port, &tmp_conf, ConfTimer);
    this_thread::sleep_for(200ms);


    /////////////////////////////////////////////////////////////////
    //�洢������
    string proto;
    auto message = CONF->LoadProto("xmsg_com.proto", "XDirConfig", proto);
    if(!message)
    {
        cerr<<"CONF->LoadProto xmsg_com.proto failed!"<<endl;
        return -1;
    }
    //ͨ����������ֵ
    auto ref = message->GetReflection();
    auto field = message->GetDescriptor()->FindFieldByName("root");
    ref->SetString(message, field, "\\test_new_root\\");
    cout << message->DebugString();

    //�洢����
    XConfig save_conf;
    save_conf.set_service_name("test_config");
    save_conf.set_service_port(client_port);
    save_conf.set_proto(proto);
    save_conf.set_private_pb(message->SerializeAsString());
    MCONF->SendConfig(&save_conf);





    /////////////////////////////////////////////////////////////////
    //��ȡ�����б� �������ߣ�
    for (;;)
    {
        //��ȡ�����б�
        auto configs = MCONF->GetAllConfig(1, 1000, 10);
        cout << configs.DebugString();
        if (configs.config_size() <= 0)
        {
            this_thread::sleep_for(2s);
            continue;
        }
        //ȡ�õ���������Ϣ(��һ��������)
        string ip = configs.config()[0].service_ip();
        int port = configs.config()[0].service_port();
        CONF->LoadConfig(ip.c_str(), port);
        XConfig conf;
        CONF->GetConfig(ip.c_str(), port, &conf);
        cout << "===========================================" << endl;
        cout << conf.DebugString() << endl;
        this_thread::sleep_for(2s);
    }

    //XConfigClient::Get()->RegMsgCallback();
    //XConfigClient::Get()->set_server_ip("127.0.0.1");
    //XConfigClient::Get()->set_server_port(CONFIG_PORT);
    //XConfigClient::Get()->StartConnect();
    //if (!XConfigClient::Get()->WaitConnected(10))
    //{
    //    cout << "������������ʧ��" << endl;
    //}
    //cout << "�����������ĳɹ�" << endl;
    //XConfig conf;
    //conf.set_service_name("test_client_name");
    //conf.set_service_ip("127.0.0.1");
    //conf.set_service_port(20030);
    //conf.set_proto("message");
    //conf.set_private_pb("test 1pb1");
    //XConfigClient::Get()->SendConfig(&conf);

    //XConfigClient::Get()->LoadConfig("127.0.0.1", 20030);

    //this_thread::sleep_for(300ms);
    //XConfig tmp_conf;
    //XConfigClient::Get()->GetConfig("127.0.0.1", 20030, &tmp_conf);
    //cout << "tmp_conf = " << tmp_conf.DebugString() << endl;

    //auto confs = XConfigClient::Get()->GetAllConfig(1, 1000, 10);
    //cout << confs.DebugString();
    //

    XConfigClient::Get()->Wait();
    return 0;
}
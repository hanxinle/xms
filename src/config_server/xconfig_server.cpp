#include "xconfig_server.h"
#include "xtools.h"
#include "xregister_client.h"
#include "xconfig_handle.h"
using namespace std;

XConfigServer::XConfigServer()
{
}


XConfigServer::~XConfigServer()
{
}
///���ݲ��� ��ʼ��������Ҫ�ȵ���
void XConfigServer::main(int argc, char *argv[])
{
    ///ע�����ĵ�����
    LOGDEBUG("config_server register_ip register_port  service_port ");

    ///ע��ص�����
    XConfigHandle::RegMsgCallback();
    int service_port = CONFIG_PORT;
    int register_port = REGISTER_PORT;
    //string register_ip = "127.0.0.1";
    string register_ip = XGetHostByName(API_REGISTER_SERVER_NAME);
    if (argc > 1)
        register_ip = argv[1];
    if(argc>2)
        register_port = atoi(argv[2]);
    if (argc > 3)
        service_port = atoi(argv[3]);

    //���÷����������˿�
    set_server_port(service_port);

    
    XRegisterClient::Get()->set_server_ip(register_ip.c_str());
    XRegisterClient::Get()->set_server_port(register_port);

    //��ע������ע��
    XRegisterClient::Get()->RegisterServer(CONFIG_NAME, service_port, 0);
}
XServiceHandle* XConfigServer::CreateServiceHandle()
{
    return new XConfigHandle();
}

//�ȴ��߳��˳�
void XConfigServer::Wait()
{
    XThreadPool::Wait();
}
#include <iostream>
#include "xregister_client.h"
#include "xtools.h"
#include <thread>
using namespace std;
int main(int argc, char *argv[])
{
    cout << "Register Client " << endl;
    cout << "register_client 127.0.0.1 20018 " << endl;
    //ע������IP �˿�
    //
    string ip = "127.0.0.1";
    int port = REGISTER_PORT;
    if (argc > 1)
    {
        ip = argv[1];
    }
    if (argc > 2)
    {
        port = atoi(argv[2]);
    }
    XRegisterClient::Get()->set_server_ip(ip.c_str());
    XRegisterClient::Get()->set_server_port(port);
    XRegisterClient::Get()->RegisterServer("test", 20020, 0);
    XRegisterClient::Get()->WaitConnected(3);
    //XThreadPool::Wait();

    //���ͻ�ȡȫ�����������
    //XRegisterClient::Get()->GetServiceReq(NULL);
    //XRegisterClient::Get()->GetServiceReq("test");
    for (;;)
    {
        XRegisterClient::Get()->GetServiceReq(NULL);
        //XRegisterClient::Get()->GetServiceReq("test");
        auto services = XRegisterClient::Get()->GetAllService();
        if(services)
            LOGINFO(services->DebugString());
        this_thread::sleep_for(5s);
    }

    
    XThreadPool::Wait();
    return 0;
}


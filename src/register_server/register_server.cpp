 #include <iostream>
#include "xregister_server.h"

using namespace std;
int main(int argc, char *argv[])
{
    cout << "Register Server" << endl;
    
    XRegisterServer server;
    //��ʼ�� ���ݲ������˿ں� register_server 20018
    server.main(argc, argv);

    //���������̣߳���ʼ�����˿�
    server.Start();
   
    //�������ȴ��̳߳��˳�
    server.Wait();
    return 0;
}
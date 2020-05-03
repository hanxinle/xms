#include "xregister_server.h"
#include "xregister_handle.h"
#include "xlog_client.h"
///���ݲ��� ��ʼ��������Ҫ�ȵ���
void XRegisterServer::main(int argc, char *argv[])
{
    //ע����Ϣ�ص�����
    XRegisterHandle::RegMsgCallback();

    int port = REGISTER_PORT;
    if (argc > 1)
        port = atoi(argv[1]);

    //��Ҫ���Ǵ�ע�������Լ���ȡ��־ģ��
    XLogClient::Get()->set_service_name("REGISTER");
    XLogClient::Get()->set_server_port(XLOG_PORT);
    XLogClient::Get()->set_auto_connect(true);
    XLogClient::Get()->StartLog();

    //���÷����������˿�
    set_server_port(port);
}
XServiceHandle* XRegisterServer::CreateServiceHandle()
{
    auto handle = new XRegisterHandle();
    //�趨��ʱ�����ڽ���������
    handle->set_read_timeout_ms(5000);
    return handle;
}

//�ȴ��߳��˳�
void XRegisterServer::Wait()
{
    XThreadPool::Wait();
}

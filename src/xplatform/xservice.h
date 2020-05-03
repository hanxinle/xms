#pragma once
#include "xtask.h"
#include "xservice_handle.h"
#include "xthread_pool.h"
class XCOM_API XService :public XTask
{
public:
    XService();
    ~XService();

    //��Ҫ���أ�ÿ�����ӽ��룬���ô˺�������������󣬼��뵽�̳߳�
    virtual XServiceHandle* CreateServiceHandle() = 0;
    
    ///�����ʼ�� ���̳߳ص���
    bool Init();

    ///��ʼ�������У���������������뵽�̳߳�
    bool Start();

    //�����������˿�
    void set_server_port(int port) { this->server_port_ = port; }

    //�������ӵ�����ļ�
    void ListenCB(int client_socket, struct sockaddr *addr, int socklen);

    //�ȴ��̳߳��˳�
    void Wait();

private:
    
    //�����û����ӵ��̳߳�
    XThreadPool *thread_listen_pool_ = 0;

    //�����û������ݵ����ӳ�
    XThreadPool *thread_client_pool_ = 0;

    //�ͻ����ݴ�����߳�����
    int thread_count_ = 10;

    //�����������˿�
    int server_port_ = 0;


};


#pragma once
#include "xservice.h"
class XConfigServer :public XService
{
public:
    XConfigServer();
    ~XConfigServer();
    
    ///���ݲ��� ��ʼ��������Ҫ�ȵ���
    void main(int argc, char *argv[]);

    //�ȴ��߳��˳�
    void Wait();

    XServiceHandle* CreateServiceHandle();

};


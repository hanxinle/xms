#ifndef XREGISTERSERVER_H
#define XREGISTERSERVER_H
#include "xservice.h"

////////////////////////////////////
//// ע�����ķ����
class XRegisterServer:public XService
{
public:

    ///���ݲ��� ��ʼ��������Ҫ�ȵ���
    void main(int argc, char *argv[]);

    //�ȴ��߳��˳�
    void Wait();

    XServiceHandle* CreateServiceHandle();

};


#endif
#pragma once

#include "xservice.h"
class XLogServer:public XService
{
public:
    ///���ݲ��� ��ʼ��������Ҫ�ȵ���
    void main(int argc, char *argv[]);


    XServiceHandle* CreateServiceHandle();
    XLogServer();
    ~XLogServer();
};


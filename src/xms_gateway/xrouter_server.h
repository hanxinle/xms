#ifndef XROUTERSERVER_H
#define XROUTERSERVER_H
#include "xservice.h"
class XSSLCtx;

class XRouterServer :public XService
{
public:
    XServiceHandle* CreateServiceHandle();
private:
    //�Ƿ������ssl
    //bool is_ssl_ = false;
    XSSLCtx *ssl_ctx_ = 0; //û���������Ĵ���
};

#endif

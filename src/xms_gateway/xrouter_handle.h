#ifndef XROUTER_HANDLE_H
#define XROUTER_HANDLE_H
#include "xservice_handle.h"
class XRouterHandle : public XServiceHandle
{
public:
    XRouterHandle();
    ~XRouterHandle();
    
    virtual void ReadCB(xmsg::XMsgHead *head, XMsg *msg);

    //���ӶϿ�����ʱ���������
    virtual void Close();

    static void InitMsgServer();

    static void CloseMsgServer();
    
    //���� ����proxy�ص�
    bool  SendMsg(xmsg::XMsgHead *head, XMsg *msg);

};

#endif
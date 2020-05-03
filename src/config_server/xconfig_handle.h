#pragma once
#include "xservice_handle.h"
class XConfigHandle :
    public XServiceHandle
{
public:
    XConfigHandle();
    ~XConfigHandle();

    //���յ��������õ���Ϣ
    void SaveConfig(xmsg::XMsgHead *head, XMsg *msg);

    //��������
    void LoadConfig(xmsg::XMsgHead *head, XMsg *msg);


    //����ȫ�����ã��з�ҳ��
    void LoadAllConfig(xmsg::XMsgHead *head, XMsg *msg);

    //ɾ�����ã�
    void DeleteConfig(xmsg::XMsgHead *head, XMsg *msg);

    static void RegMsgCallback()
    {
        RegCB(xmsg::MSG_SAVE_CONFIG_REQ, (MsgCBFunc)&XConfigHandle::SaveConfig);
        RegCB(xmsg::MSG_LOAD_CONFIG_REQ, (MsgCBFunc)&XConfigHandle::LoadConfig);
        RegCB(xmsg::MSG_LOAD_ALL_CONFIG_REQ, (MsgCBFunc)&XConfigHandle::LoadAllConfig);
        RegCB(xmsg::MSG_DEL_CONFIG_REQ, (MsgCBFunc)&XConfigHandle::DeleteConfig);
    }
};


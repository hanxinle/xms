#pragma once
#include "xmsg_type.pb.h"
#include "xmsg.h"
#include "xcom_task.h"
#include "xmsg_com.pb.h"


//������bufferevent�ӿڣ�ֱ�ӵ���XComTask�ķ�װ
class XCOM_API XMsgEvent:public XComTask
{
public:
   
    virtual void DropInMsg() { is_drop_ = true; };

    ////������Ϣ���ַ���Ϣ
    virtual void ReadCB();

    //��Ϣ�ص�������Ĭ�Ϸ��͵��û�ע��ĺ�����·������
    //����false ���˳�������Ϣѭ��
    virtual void ReadCB(xmsg::XMsgHead *head, XMsg *msg);





    //////////////////////////////////
    /// ������Ϣ������ͷ�����Զ�������
    /// @type ��Ϣ����
    /// @message ��Ϣ����
    /// @return ���ʹ��󣬱���bevδ����
    virtual bool  SendMsg(xmsg::MsgType type, const google::protobuf::Message *message);
    virtual bool  SendMsg(xmsg::XMsgHead *head, const google::protobuf::Message *message);
    virtual bool  SendMsg(xmsg::XMsgHead *head, XMsg *msg);
    //virtual bool  SendLog(const google::protobuf::Message *message);

    /////////////////////////////////////
    /// ��������Ϣͷ����Ϣ���ݣ����ڽ�����һ����Ϣ
    void Clear();

    void Close();

    typedef void (XMsgEvent::*MsgCBFunc) (xmsg::XMsgHead *head, XMsg *msg);
    
    ////////////////////////////////////////////////////
    /// �����Ϣ����Ļص�������������Ϣ���ͷַ� ,ͬһ������ֻ����һ���ص�����
    /// @para type ��Ϣ����
    /// @para func ��Ϣ�ص�����
    static void RegCB(xmsg::MsgType type, MsgCBFunc func);


private:

    //////////////////////////////////////////
    /// �������ݰ���
    /// 1 ��ȷ���յ���Ϣ  (������Ϣ������)
    /// 2 ��Ϣ���ղ����� (�ȴ���һ�ν���) 
    /// 3 ��Ϣ���ճ��� ���˳�����ռ䣩
    /// @return 1 2 ����true 3����false
    bool RecvMsg();

    /////////////////////////////////////////
    /// ��ȡ���յ������ݰ�����������ͷ����Ϣ��
    /// �ɵ���������XMsg
    /// @return ���û�����������ݰ�������NULL
    XMsg *GetMsg();

    bool is_drop_ = false;
    XMsg head_; //��Ϣͷ
    XMsg msg_;  //��Ϣ����

    //pb��Ϣͷ
    xmsg::XMsgHead *pb_head_ = 0;
};


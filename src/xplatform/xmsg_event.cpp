#include "xmsg_event.h"
#include "xmsg_com.pb.h"
#include "xtools.h"
#include "xlog_client.h"
#include <iostream>
#include <sstream>
#include <map>
using namespace std;
using namespace xmsg;
using namespace google;
using namespace protobuf;

//ͬһ������ֻ����һ���ص�����
static map< MsgType, XMsgEvent::MsgCBFunc> msg_callback;
void XMsgEvent::RegCB(xmsg::MsgType type, XMsgEvent::MsgCBFunc func)
{
    if (msg_callback.find(type) != msg_callback.end())
    {
        stringstream ss;
        ss << "RegCB is error," << type << " have been set " << endl;
        LOGERROR(ss.str().c_str());
        return;
    }
    msg_callback[type] = func;
}

void XMsgEvent::ReadCB(xmsg::XMsgHead *head, XMsg *msg)
{
    //�ص���Ϣ����
    auto ptr = msg_callback.find(head->msg_type());
    if (ptr == msg_callback.end())
    {
        Clear();
        stringstream ss;
        ss << "msg error func not set!";
        ss << head->msg_type();
        LOGDEBUG(ss.str().c_str());
        return;
    }
    auto func = ptr->second;
    (this->*func)(pb_head_, msg);
}
////������Ϣ���ַ���Ϣ
void XMsgEvent::ReadCB()
{
    static int i = 0;
    i++;
    //cout << "<" << i << ">" << flush;
    //����߳��˳�
    while (1)
    {
       // cout << "#" << flush;
        if (!RecvMsg())
        {
            Clear();
            break;
        }
        if (!pb_head_)break;
        auto msg = GetMsg();
        if (!msg)break;

        if (pb_head_)
        {
            //cout << "��MSG��" << pb_head_->service_name() << " " << msg->size << " " << msg->type << endl;
            stringstream ss;
            ss << "��RECV��"<<server_ip()<<":"<<server_port()<<"|"<< XGetPortName(server_port())<<" " <<client_ip()<<":"<<client_port()<<" " << pb_head_->DebugString();
            //cout << ss.str() << endl;
            if (pb_head_->msg_type() != xmsg::MSG_ADD_LOG_REQ)
                LOGDEBUG(ss.str().c_str());
        }

        //cout << "service_name = " << pb_head_->service_name() << endl;
        //���������ʲ���ȫ
        //LOGDEBUG(pb_head_->service_name());
        //msg->head = pb_head_;
        ReadCB(pb_head_, msg);

        Clear();
        if (is_drop_)
        {
            set_auto_delete(true);
            Close();
            return;
        }
    }
    //cout << "<" << 0 << ">" << flush;
}
//////////////////////////////////////////
/// �������ݰ���
/// 1 ��ȷ���յ���Ϣ  (������Ϣ������)
/// 2 ��Ϣ���ղ����� (�ȴ���һ�ν���) 
/// 3 ��Ϣ���ճ��� ���˳�����ռ䣩
/// @return 1 2 ����true 3����false
bool XMsgEvent::RecvMsg()
{

    //���
    
    //������Ϣͷ
    if (!head_.size)
    {
        //1 ��Ϣͷ��С
        int len = Read(&head_.size, sizeof(head_.size));//bufferevent_read(bev_, &head_.size, sizeof(head_.size));
        if (len <= 0 || head_.size <= 0)
        {
            return false;
        }

        //������Ϣͷ�ռ� ��ȡ��Ϣͷ����Ȩ����Ϣ��С��
        if (!head_.Alloc(head_.size))
        {
            stringstream ss;
            ss<< "head_.Alloc failed!" << head_.size;
            LOGDEBUG(ss.str().c_str());
            return false;
        }
    }
    //2 ��ʼ������Ϣͷ����Ȩ����Ϣ��С��
    if (!head_.Recved())
    {
        int len = Read(
            head_.data + head_.recv_size,  //�ڶ��ν��� ���ϴε�λ�ÿ�ʼ��
            head_.size - head_.recv_size
        );
        if (len <= 0)
        {
            return true;
        }
        head_.recv_size += len;

        if (!head_.Recved())
            return true;

        //������ͷ�����ݽ������
        //�����л�
        if (!pb_head_)
        {
            pb_head_ = new XMsgHead();
        }
        if (!pb_head_->ParseFromArray(head_.data, head_.size))
        {
            stringstream ss;
            ss << "pb_head.ParseFromArray failed!" <<head_.size;

            LOGDEBUG(ss.str().c_str());
            return false;
        }
        
        //�հ�����
        if (pb_head_->msg_size() == 0)
        {
           // cout << "0" << flush;
            //��Ϣ����
            msg_.type = pb_head_->msg_type();
            msg_.size = 0;
            return true;
        }
        else
        {
            //��Ȩ
            //��Ϣ���ݴ�С
            //������Ϣ���ݿռ�
            if (!msg_.Alloc(pb_head_->msg_size()))
            {
                stringstream ss;
                ss<<"msg_.Alloc failed!msg_size="<<pb_head_->msg_size();

                LOGDEBUG(ss.str().c_str());
                return false;
            }
        }

        //��Ϣ����
        msg_.type = pb_head_->msg_type();
    }

    //3 ��ʼ������Ϣ����
    if (!msg_.Recved())
    {
        int len = Read(
            msg_.data + msg_.recv_size,  //�ڶ��ν��� ���ϴε�λ�ÿ�ʼ��
            msg_.size - msg_.recv_size
        );
        if (len <= 0)
        {
            return true;
        }
        msg_.recv_size += len;
    }

    if (msg_.Recved())
    {
        cout << "+" << flush;
    }

    return true;
}

/////////////////////////////////////////
/// ��ȡ���յ������ݰ�����������ͷ����Ϣ��
/// �ɵ���������XMsg
/// @return ���û�����������ݰ�������NULL
XMsg *XMsgEvent::GetMsg()
{
    if (msg_.Recved())
        return &msg_;
    return NULL;
}


void XMsgEvent::Close()
{
    Clear();
    XComTask::Close();
}
/////////////////////////////////////
/// ��������Ϣͷ����Ϣ���ݣ����ڽ�����һ����Ϣ
void XMsgEvent::Clear()
{
    head_.Clear();
    msg_.Clear();
}

bool  XMsgEvent::SendMsg(xmsg::XMsgHead *head, XMsg *msg)
{
    //if (!head || !msg)
    if (!head ) //֧��ֻ����ͷ��
        return false;
    head->set_msg_size(msg->size);
    //��Ϣͷ���л�
    string head_str = head->SerializeAsString();
    int headsize = head_str.size();
    //stringstream ss;
    //ss << "SendMsg" << head->msg_type();
    //LOGDEBUG(ss.str());
    if (head)
    {
        stringstream ss;
        ss << "��SEND��"<<server_ip()<<":"<<server_port() <<" "<< XGetPortName(server_port()) << " " << head->DebugString();
        //cout << ss.str() << endl;
        // �ڼ�¼��־����ѭ��
        if(head->msg_type() != xmsg::MSG_ADD_LOG_REQ)
            LOGDEBUG(ss.str().c_str());
    }
        

    //1 ������Ϣͷ��С 4�ֽ� ��ʱ�������ֽ�������
    int re = Write(&headsize, sizeof(headsize));
    if (!re)return false;

    //2 ������Ϣͷ��pb���л��� XMsgHead ��������Ϣ���ݵĴ�С��
    re = Write(head_str.data(), head_str.size());
    if (!re)return false;

   
    //֧�ַ��Ϳհ�
    if (msg->size > 0)
    {
        re = Write(msg->data, msg->size);
        if (!re)return false;
    }
    return true;
}

bool XMsgEvent::SendMsg(xmsg::XMsgHead *head, const Message *message)
{
    if (!message || !head)
        return false;
    ///���
    //��Ϣ�������л�
    string msg_str = message->SerializeAsString();
    int msg_size = msg_str.size();
    XMsg msg;
    msg.data = (char*)msg_str.data();
    msg.size = msg_size;
    return SendMsg(head, &msg);
    //head->set_msg_size(msg_size);

    ////��Ϣͷ���л�
    //string head_str = head->SerializeAsString();
    //int headsize = head_str.size();

    ////1 ������Ϣͷ��С 4�ֽ� ��ʱ�������ֽ�������
    //int re = Write(&headsize, sizeof(headsize));
    //if (!re)return false;

    ////2 ������Ϣͷ��pb���л��� XMsgHead ��������Ϣ���ݵĴ�С��
    //re = Write(head_str.data(), head_str.size());
    //if (!re)return false;

    ////3 ������Ϣ���� ��pb���л��� ҵ��proto
    //re = Write(msg_str.data(), msg_str.size());
    //if (!re)return false;
    //return true;
}
bool XMsgEvent::SendMsg(MsgType type, const Message *message)
{
    if (!message)
        return false;
    XMsgHead head;
    head.set_msg_type(type);
    return SendMsg(&head, message);
}

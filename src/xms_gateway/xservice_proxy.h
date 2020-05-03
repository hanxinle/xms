#ifndef XSERVICEPROXY_H
#define XSERVICEPROXY_H
#include <map>
#include <vector>
#include <string>
#include "xservice_proxy_client.h"
class XServiceProxy
{
public:
    static XServiceProxy *Get()
    {
        static XServiceProxy xs;
        return &xs;
    }
    XServiceProxy();
    ~XServiceProxy();
    ///��ʼ��΢�����б�ע�����Ļ�ȡ������������
    bool Init();

    //���ؾ����ҵ��ͻ������ӣ��������ݷ���
    bool SendMsg(xmsg::XMsgHead *head, XMsg *msg,XMsgEvent *ev);

    //������Ϣ�ص�
    void DelEvent(XMsgEvent *ev);


    //�����Զ��������߳�
    void Start();

    //ֹͣ�߳�
    void Stop();

    void Main();

private:

    bool is_exit_ = false;
    //��������΢��������Ӷ���
    std::map < std::string, std::vector<XServiceProxyClient *>> client_map_;

    std::mutex client_map_mutex_;
    //��¼�ϴ���ѯ������
    std::map<std::string, int>client_map_last_index_;


    //��������callback����
    std::map<XMsgEvent*, XServiceProxyClient*> callbacks_;
    std::mutex callbacks_mutex_;


};

#endif
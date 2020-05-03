#pragma once
#include "xmsg_type.pb.h"

#ifdef _WIN32
#ifdef XCOM_EXPORTS
#define XCOM_API __declspec(dllexport)
#else
#define XCOM_API __declspec(dllimport)
#endif
#else
#define XCOM_API
#endif

//ͷ����Ϣ������ֽ���
//#define MAX_MSG_SIZE 8192
#define MAX_MSG_SIZE 100000000  //һ�������100M
#define API_GATEWAY_PORT 20010

/*
    windows�� C:\Windows\System32\drivers\etc
192.168.1.205 xms_gateway_server
192.168.1.205 xms_register_server
*/
#define API_GATEWAY_SERVER_NAME "xms_gateway_server"
#define API_GATEWAY_NAME "gw"

#define REGISTER_PORT 20018
#define REGISTER_NAME "reg"
#define API_REGISTER_SERVER_NAME "xms_register_server"


#define CONFIG_NAME "config"
#define CONFIG_PORT  20019
#define AUTH_NAME "auth"
#define AUTH_PORT 20020
#define XLOG_NAME "xlog"
#define XLOG_PORT 20030
#define UPLOAD_PORT 20100
#define UPLOAD_NAME "upload"
#define DOWNLOAD_PORT 20200
#define DOWNLOAD_NAME "download"
#define DIR_PORT 20300
#define DIR_NAME "dir"




//���еĺ���������
class XMsg
{
public:
    //���ݴ�С
    int size = 0;

    //��Ϣ����
    xmsg::MsgType type = xmsg::NONE_DO_NOT_USE;

    //���ݴ�ţ�protobuf�����л�������ݣ�
    char *data = 0;

    //�Ѿ����յ����ݴ�С
    int recv_size = 0;

    //ƫ��λ��
    //int offset = 0; 


    bool Alloc(int s)
    {
        if (s <= 0 || s > MAX_MSG_SIZE)
        {
            std::cout << "alloc msg size > MAX_MSG_SIZE:" << MAX_MSG_SIZE << std::endl;
            return false;
        }
            
        if (data)
            delete data;
        data = new char[s];
        if (!data)
            return false;
        this->size = s;
        this->recv_size = 0;
        return true;
    }

    //�ж������Ƿ�������
    bool Recved()
    {
        //���տհ�
        if (size < 0)return false;
        return (recv_size == size);
    }

    void Clear()
    {
        if (size > 0)
        {
            delete data;
            data = NULL;
        }
        memset(this, 0, sizeof(XMsg));
    }
};


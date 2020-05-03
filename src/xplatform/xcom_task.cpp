#include "xcom_task.h"

#include "xlog_client.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <iostream>
#include <string.h>
#include "xtools.h"
#include "event2/bufferevent_ssl.h"
#include <chrono>
#include <thread>
#include "xssl.h"
using namespace std;

XCOM_API const char * XGetPortName(unsigned short port)
{
    switch (port)
    {
    case API_GATEWAY_PORT:
        return API_GATEWAY_NAME;
        break;
    case REGISTER_PORT:
        return REGISTER_NAME;
        break;
    case CONFIG_PORT:
        return AUTH_NAME;
        break;
    case XLOG_PORT:
        return XLOG_NAME;
        break;
    case DOWNLOAD_PORT:
        return DOWNLOAD_NAME;
        break;
    case DIR_PORT:
        return DIR_NAME;
        break;
    case UPLOAD_PORT:
        return UPLOAD_NAME;
        break;
    default:
        break;
    }
    return "";
}

static void SReadCB(struct bufferevent *bev, void *ctx)
{
    auto task = (XComTask*)ctx;
    task->ReadCB();
    //static int i = 0;
    //i++;
    //cout << "{"<<i<<"}" << flush;
}
static void SWriteCB(struct bufferevent *bev, void *ctx)
{
    auto task = (XComTask*)ctx;
    task->WriteCB();
}

void SAutoConnectTimerCB(evutil_socket_t s, short w, void *ctx)
{
    auto task = (XComTask*)ctx;
    task->AutoConnectTimerCB();
}
void STimerCB(evutil_socket_t s, short w, void *ctx)
{
    auto task = (XComTask*)ctx;
    task->TimerCB();
}
static void SEventCB(struct bufferevent *bev, short what,void *ctx)
{
    auto task = (XComTask*)ctx;
    task->EventCB(what);
}

XComTask::XComTask()
{
    mux_ = new mutex;
}
XComTask::~XComTask()
{
    // ����mutex��Ҫ���ǣ�����ڶ���delete �����߳���lock
    // Ŀǰֻ��ͨ�ŷ���˻�������󣬿ͻ��˻��Զ����������Բ����������
    // ����˵����������̳߳��д�������رպ󣬲������ô˻���
    // ����Ҫ�������صĴ���
    delete mux_;
    mux_ = NULL;
}
//���ô����̲߳���ȫ
void XComTask::set_error(const char * err)
{
    if (!err)return;
    strncpy(error_, err, sizeof(error_));
}
void XComTask::set_client_ip(const char*ip)
{
    if (!ip)return;
    strncpy(client_ip_, ip, sizeof(client_ip_));
}

//////////////////////////////////////////////////////////////
/// �趨�Զ������Ķ�ʱ��
void XComTask::SetAutoConnectTimer(int ms)
{
    if (!base())
    {
        LOGERROR("SetAutoConnectTimer failed : base not set!");
        return;
    }
    if(auto_connect_timer_event_)
    {
        event_free(auto_connect_timer_event_);
        auto_connect_timer_event_ = 0;
    }

    auto_connect_timer_event_ = event_new(base(), -1, EV_PERSIST, SAutoConnectTimerCB, this);
    if (!auto_connect_timer_event_)
    {
        LOGERROR("SetAutoConnectTimer  failed :event_new faield!");
        return;
    }
    int sec = ms / 1000; //��
    int us = (ms % 1000) * 1000;//΢��
    timeval tv = { sec,us };
    event_add(auto_connect_timer_event_, &tv);
}
    
/////////////////////////////////////////
///�Զ�������ʱ���ص�����
void XComTask::AutoConnectTimerCB()
{
     //����������ӣ���ȴ������û�У���ʼ����
    if (is_connected())
        return ;
    if (!is_connecting())
    {
        Connect();
        cout << "." << flush;
    }
}

//////////////////////////////////////////////////////////////
///�趨��ʱ�� ֻ������һ����ʱ�� ��ʱ����TimerCB()�ص�
/// ��Init�����е���
///@para ms ��ʱ���õĺ���
void XComTask::SetTimer(int ms)
{
    if (!base())
    {
        LOGERROR("SetTimer failed : base not set!");
        return;
    }

    timer_event_ = event_new(base(), -1, EV_PERSIST, STimerCB, this);
    if (!timer_event_)
    {
        LOGERROR("set timer failed :event_new faield!");
        return;
    }
    int sec = ms / 1000; //��
    int us = (ms % 1000) * 1000;//΢��
    timeval tv = { sec,us };
    event_add(timer_event_, &tv);
}
void XComTask::set_local_ip(const char *ip)
{
    strncpy(this->local_ip_, ip, sizeof(local_ip_));
}

void XComTask::set_server_ip(const char* ip)
{
    strncpy(this->server_ip_, ip, sizeof(server_ip_));
}

int XComTask::Read(void *data, int datasize)
{
    if (!bev_)
    {
        LOGERROR("bev not set");
        return 0;
    }
    int re = bufferevent_read(bev_, data, datasize);
    if(re>0)
        recv_data_size_ += re;
    return re;
}

///�������ж�ʱ��
void XComTask::ClearTimer()
{
    if(auto_connect_timer_event_)
        event_free(auto_connect_timer_event_);
    auto_connect_timer_event_ = 0;
    if(timer_event_)
        event_free(timer_event_);
    timer_event_ = 0;
}

void XComTask::Close()
{
    {
        XMutex mux(mux_);
        is_connected_ = false;
        is_connecting_ = false;
        is_closed_ = true;
        if (bev_)
        {
            //��������� BEV_OPT_CLOSE_ON_FREE���ͷ�ssl  ��socket
            //��һ�����ͷ�����ռ�ÿռ䣬�����б��е��ͷŲ���ͨ��event_base_loop(base, EVLOOP_NONBLOCK);ȡ��
            bufferevent_free(bev_);
            bev_ = NULL;
        }

        if (msg_.data)
            delete msg_.data;
        memset(&msg_, 0, sizeof(msg_));
        //if (ssl_ctx_) �������Լ�����
        //{
        //    ssl_ctx_->Close();
        //    delete ssl_ctx_;
        //    ssl_ctx_ = 0;
        //}
    }
    //�������Ӷ���ռ䣬����Ͽ���������Ҫ��������
    if(auto_delete_)
    {
        //����ʱ��
        ClearTimer();
        delete this;
    }
        
}

//���л��壨δ���ͣ��Ĵ�С
long long XComTask::BufferSize()
{
    XMutex mux(mux_);
    if (!bev_ )return 0;
    auto evbuf = bufferevent_get_output(bev_);
    auto len = evbuffer_get_length(evbuf);
    return  len;
}

bool XComTask::Write(const void *data, int size)
{
    XMutex mux(mux_);
    if (!bev_  || !data || size <= 0)return false;
    int re = bufferevent_write(bev_, data, size);
    if (re != 0)return false;
    send_data_size_ += size;
    return true;
}

void XComTask::EventCB(short what)
{
    stringstream ss;
    ss << "SEventCB:" << what ;
    
    if (what & BEV_EVENT_CONNECTED)
    {
        //ss << "BEV_EVENT_CONNECTED " ;
        stringstream ss;
        ss << "connnect server " << server_ip_ << ":" << server_port_ << " " << XGetPortName(server_port_) << " success!";
        LOGINFO(ss.str().c_str());
        //֪ͨ���ӳɹ�
        is_connected_ = true;
        is_connecting_ = false;
        auto ssl = bufferevent_openssl_get_ssl(bev_);
        if (ssl)
        {
            XSSL xssl;
            xssl.set_ssl(ssl);
            xssl.PrintCert();
            xssl.PrintCipher();
        }
        //��ȡ���ص�ַ
        //int sock = bufferevent_getfd(bev_);
        /*if (sock > 0)
        {
            sockaddr_in sin;
            memset(&sin, 0, sizeof(sin));
            int len = sizeof(sin);
            getsockname(sock, (sockaddr*)&sin, &len);
            char buf[16] = { 0 };
            evutil_inet_ntop(AF_INET, &sin.sin_addr.s_addr, buf, sizeof(buf));
            cout << "client ip is " << buf << endl;
        }*/
        ConnectedCB();
    }

    ///�˳�Ҫ����������
    if (what & BEV_EVENT_ERROR )
    {
        auto ssl = bufferevent_openssl_get_ssl(bev_);
        if (ssl)
        {
            XSSL xssl;
            xssl.set_ssl(ssl);
            xssl.PrintCert();
        }
        ss << "BEV_EVENT_ERROR " ;
        int sock = bufferevent_getfd(bev_);
        int err = evutil_socket_geterror(sock);
        //err = evutil_socket_geterror(sock);
        //LOGDEBUG(server_ip());
        //stringstream log;
        ss<<server_ip()<<":"<<server_port()<<" " << XGetPortName(server_port());
        ss<<local_ip()<<evutil_socket_error_to_string(err);
        //LOGDEBUG(log.str().c_str());
        LOGINFO(ss.str().c_str());
        has_error_ = true;
        strcpy(error_, evutil_socket_error_to_string(err));
        Close();
    }
    if (what & BEV_EVENT_TIMEOUT)
    {
        ss << "BEV_EVENT_TIMEOUT" ;
        LOGINFO(ss.str().c_str());
        has_error_ = true;
        strcpy(error_, "BEV_EVENT_TIMEOUT");
        Close();
    }
    if (what & BEV_EVENT_EOF)
    {
        ss << "BEV_EVENT_EOF" ;
        LOGINFO(ss.str().c_str());
        Close();
    }
}

bool XComTask::Connect()
{
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port_);
    evutil_inet_pton(AF_INET, server_ip_, &sin.sin_addr.s_addr);
    XMutex mux(mux_);
    is_connected_ = false;
    is_connecting_ = false;
    
    //��ʼ��bufferevent
    if(!bev_)InitBev(-1);
    if (!bev_)
    {
        LOGERROR("XComTask::Connect failed! bev is null!");
        return false;
    }
    int re = bufferevent_socket_connect(bev_, (sockaddr*)&sin, sizeof(sin));
    if (re != 0)
    {
        return false;
    }

    //��ʼ����
    is_connecting_ = true;
    return true;
}

bool XComTask::InitBev(int sock)
{
    //mux_->lock();
    ///��bufferevent �������� 
    /// ����bufferevent������ -1�Զ�����socket
    if (!ssl_ctx())
    {
        bev_ = bufferevent_socket_new(base(), sock, BEV_OPT_CLOSE_ON_FREE);
        if (!bev_)
        {
            //mux_->unlock();
            LOGERROR("bufferevent_socket_new failed!");
            return false;
        }
    }
    else// ����ͨ��
    {
        auto xssl = ssl_ctx()->NewXSSL(sock);
        //�ͻ���
        if (sock < 0)
        {
            bev_ = bufferevent_openssl_socket_new(base(), sock, xssl.ssl()
                , BUFFEREVENT_SSL_CONNECTING,
                BEV_OPT_CLOSE_ON_FREE// bufferevent_free��ͬʱ�ر�socket��ssl
            );
        }
        else
        {//�����
            bev_ = bufferevent_openssl_socket_new(base(), sock, xssl.ssl()
                , BUFFEREVENT_SSL_ACCEPTING,
                BEV_OPT_CLOSE_ON_FREE// bufferevent_free��ͬʱ�ر�socket��ssl
            );
        }
        if (!bev_)
        {
            //mux_->unlock();
            LOGERROR("bufferevent_openssl_socket_new failed!");
            return false;
        }
    }

    //�趨����ʱʱ��
    if (read_timeout_ms_ > 0)
    {
        //�룬΢��
        timeval read_tv = { read_timeout_ms_ / 1000,
            (read_timeout_ms_ % 1000) * 1000 };
        bufferevent_set_timeouts(bev_, &read_tv, 0);
    }

    //��ʱ���趨
    if (timer_ms_ > 0)
    {
        SetTimer(timer_ms_);
    }

    //���ûص�����
    bufferevent_setcb(bev_, SReadCB, SWriteCB, SEventCB, this);
    bufferevent_enable(bev_, EV_READ | EV_WRITE);
    //mux_->unlock();
    return true;
}

//��ӵ������б����
bool XComTask::Init()
{
    //���ַ���˻��ǿͻ���
    int comsock = this->sock();
    if (comsock <= 0)
        comsock = -1;
    {
        XMutex mux(mux_);
        InitBev(comsock);
    }
    //timeval tv = { 10,0 };
    //bufferevent_set_timeouts(bev_, &tv, &tv);

    //���ӷ�����
    if (server_ip_[0] == '\0')
    {
        return true;
    }
    
    //�Ͽ������Զ�����
    SetAutoConnectTimer(3000);

    //�ͻ���
    return Connect();
}

//////////////////////////////////////////////////////
///�������ӣ�����Ͽ������ٴ�������֪�����ӳɹ������߳�ʱ
bool XComTask::AutoConnect(int timeout_sec)
{
    //����������ӣ���ȴ������û�У���ʼ����
    if (is_connected())
        return true;
    if (!is_connecting())
        Connect();
    return WaitConnected(timeout_sec);
}
//////////////////////////////////////////////////////////////
///�ȴ����ӳɹ�
///@para timeout_sec ���ȴ�ʱ��
bool  XComTask::WaitConnected(int timeout_sec)
{
    //10�������һ��
    int count = timeout_sec * 100;
    for (int i = 0; i < count; i++)
    {
        if (is_connected())
            return true;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    return is_connected();
}

#ifndef XCOM_TASK_H
#define XCOM_TASK_H
#include "xtask.h"
#include "xmsg.h"
#include <string>
#include "xssl_ctx.h"

XCOM_API const char * XGetPortName(unsigned short port);

class XSSLCtx;

class XCOM_API XComTask : public XTask
{
public:

    XComTask();

    virtual ~XComTask();
    
    /////////////////////////////////////////////////////////////////
    /// ��ʼ���ӷ����������ó�Ա server_ip_ server_port_
    /// Ҫ�� ͨ��set_server_ip set_server_port �趨IP�Ͷ˿�
    /// set_auto_connect �趨�Ƿ��Զ�����
    /// @return bev����δ��������false������true���������ӳɹ�,��ʾ�����������ɹ�
    virtual bool Connect();

    /////////////////////////////////////////////////////////////////
    ///��ʼ��bufferevent���ͻ��˽�������
    ///��ӵ��̳߳������б���ã������ͻ��˺ͷ����
    /// @return ����˷���true���ͻ��˵���Connect������
    virtual bool Init();


    /////////////////////////////////////////////////////////////////
    /// ������Դ������ռ��ͷŸ���auto_delete_
    /// ���������auto_delete_ ��delete��ǰ���󣬲�����ʱ��
    /// ���δ����auto_delete_ һ�������Զ������Ŀͻ��ˣ�֪����bev�ͻ�������
    virtual void Close();



    ///�趨�ͻ�ȡ��������IP �������ӷ������Ŀͻ���
    void set_server_ip(const char* ip);
    const char *server_ip() { return server_ip_; }
    
    ///�趨�ͻ�ȡ��������Port �������ӷ������Ŀͻ���
    void set_server_port(int port) { this->server_port_ = port; }
    int server_port() { return this->server_port_; }
    
    //����IP���ڻ�ȡ������
    // �ͻ��������ӳɹ������� ���Ƿ���˵Ľ������ӵĿͻ���IP��client_ip()��
    void set_local_ip(const char *ip);
    const char *local_ip() { return local_ip_; };


    ///�¼��ص�����
    virtual void EventCB(short what);

    ////////////////////////////////////////////////////////////
    /// ��ȡbev�е�����
    /// @para data ��ȡ�û���
    /// @para size ����ռ��С���ֽ�����
    /// @return ���ض�ȡ�����ݵ��ֽ���
    int Read(void *data, int datasize);

    ////////////////////////////////////////////////////////////
    /// ������Ϣ
    /// @para data ����
    /// @para size ���ݴ�С���ֽ�����
    /// @return ����true��ʾbevд�뻺��ɹ������ܱ�֤���ͳɹ�
    virtual bool Write(const void *data, int size);

    ///���л��壨δ���ͣ��Ĵ�С
    virtual long long BufferSize();

    ///���ӳɹ�����Ϣ�ص�����ҵ��������
    virtual void ConnectedCB() {};

    /////////////////////////////////////////////////////////
    ///���ر���Ϣ����ʱ�����ݽ����͵��˺�������ҵ��ģ������
    virtual void ReadCB(void *data, int size) {}

    ///д�����ݻص�����
    virtual void WriteCB() {};

    ////////////////////////////////////////
    /// �������������ǻ���ô˴��麯��
    /// ��ȡ���ݻص�����
    /// ��Ҫ�û����أ��û������غ����е��� Read������ȡ����
    virtual void ReadCB() = 0;


    //////////////////////////////////////////////////////////////
    ///�ȴ����ӳɹ�
    ///@para timeout_sec ���ȴ�ʱ��
    bool WaitConnected(int timeout_sec);

    //////////////////////////////////////////////////////
    ///�������ӣ�����Ͽ������ٴ�������֪�����ӳɹ������߳�ʱ
    bool AutoConnect(int timeout_sec);

    ///�Ƿ���������
    bool is_connecting() { return is_connecting_; }

    ///�Ƿ����ӳɹ�
    bool is_connected() { return is_connected_; }

    //���ӶϿ��Ƿ��Զ�������� ��������ʱ��
    void set_auto_delete(bool is) { auto_delete_ = is; }

    //�Ƿ��Զ����� Ĭ�ϲ��Զ� Ҫ����ӵ��̳߳�֮ǰ��
    //�����Զ����� ����Ͳ����Զ�����
    void set_auto_connect(bool is) 
    {
        auto_connect_ = is;
        if(is)//�Զ����� ����Ͳ����Զ�����
            auto_delete_ = false;
    }

    //////////////////////////////////////////////////////////////
    ///�趨��ʱ�� ֻ������һ����ʱ�� ��ʱ����TimerCB()�ص�
    /// ��Init�����е��� �����ӽ���ǰ������Ч������ͨ��set_timer_ms����������
    ///@para ms ��ʱ���õĺ���
    virtual void SetTimer(int ms);

    ///�������ж�ʱ��
    virtual void ClearTimer();
    
    /////////////////////////////////////////
    ///��ʱ���ص�����
    virtual void TimerCB() {}

    //////////////////////////////////////////////////////////////
    /// �趨�Զ������Ķ�ʱ��
    virtual void SetAutoConnectTimer(int ms);

    /////////////////////////////////////////
    ///�Զ�������ʱ���ص�����
    virtual void AutoConnectTimerCB() ;

    ///�趨Ҫ�ڼ����̳߳�֮ǰ
    void set_read_timeout_ms(int ms) { read_timeout_ms_ = ms; }
    
    //�趨Ҫ�ڼ����̳߳�֮ǰ virtual void TimerCB() {}
    void set_timer_ms(int ms) { timer_ms_ = ms; }

    ///���������յ����ӣ��ͻ��˵�IP�Ͷ˿�
    void set_client_ip(const char*ip);
    const char *client_ip() { return client_ip_; }
    const int client_port() { return client_port_; }
    void set_client_port(int port) { this->client_port_ = port; }


    bool is_closed() { return is_closed_; } //�Ƿ��ѹرգ����ڷ��Զ����ӣ�������ռ�

    ///�Ƿ��д���
    bool has_error() { return has_error_; }  //�Ƿ��г���
    
    ///����ԭ�� �̲߳���ȫ
    const char *error() { return error_; };  
    
    // �Ѿ�д�뻺�� ��XMsg *msg �����ֽڴ�С ��������Ϣͷ
    long long send_data_size() { return send_data_size_; }

    // �Ѿ����յ��ֽڴ�С
    long long recv_data_size() { return recv_data_size_; }

protected:

    //���ô����̲߳���ȫ
    void set_error(const char * err);

    ///���������յ����ӣ���ſͻ��˵�IP
    char client_ip_[16] = { 0 };

    ///���������յ����ӣ���ſͻ��˵�Port
    int client_port_ = 0;

    //��ȡ����
    char read_buf_[4096] = { 0 };

    //����IP���ڻ�ȡ������
    char local_ip_[16] = { 0 };
    
    //XSSL *ssl_ = 0;
   // struct ssl_st *ssl_ = 0;
private:

    // �Ѿ�д�뻺�� ��XMsg *msg �����ֽڴ�С
    long long send_data_size_ = 0;

    // �Ѿ����յ��ֽڴ�С
    long long recv_data_size_ = 0;

    //TimerCB ��ʱ����ʱ��
    int timer_ms_ = 0;

    //����ʱʱ�䣬����
    int read_timeout_ms_ = 0;

    //�Ƿ��Զ�����
    bool auto_connect_ = false;

    //��ʱ������ closeʱ������
    struct event * auto_connect_timer_event_ = 0;

    //���ӶϿ��Ƿ��Զ��������
    bool auto_delete_ = true;

    /// ��ʼ�� bev�������ģ�����ssl��ʼ��
    bool InitBev(int sock);

    /// ������IP
    char server_ip_[16] = {0};

    ///�������˿�
    int server_port_ = 0;

    struct bufferevent *bev_ = 0;

    //���ݰ�����
    XMsg msg_;

    //�ͻ���������״̬ 
    //1 δ����  => ��ʼ���� �����뵽�̳߳ش���
    //2 ������ => �ȴ����ӳɹ�
    //3 ������ => ��ҵ�����
    //4 ���Ӻ�ʧ�� => �������Ӽ��ʱ�䣬��ʼ����
    bool is_connecting_ = true; //������
    bool is_connected_ = false;
    bool is_closed_ = false;    //�Ƿ�ر��ˣ�ֻ�з��Զ��������õ��˲���
    bool has_error_ = false;     //�Ƿ��г���
    char error_[1024] = { 0 };  //����ԭ��
    std::mutex * mux_ = 0;

    //��ʱ������ closeʱ������
    struct event * timer_event_ = 0;
};

#endif
#include "xrouter_server.h"
#include "xrouter_handle.h"
#include "xconfig_client.h"
#include "xtools.h"
#include "xlog_client.h"
#include <string>
using namespace std;
XServiceHandle* XRouterServer::CreateServiceHandle()
{
    //��Ҫ���ssl������������
    // OPENSSL_Uplink(57CB3350,08): no OPENSSL_Applink
    // ���� C:\xdisk_lesson\include\openssl\applink.c
    string crt_path = XConfigClient::Get()->GetString("crt_path");
    string key_path = XConfigClient::Get()->GetString("key_path");
    string ca_path = XConfigClient::Get()->GetString("ca_path");
    cout<<"crt_path = "<<crt_path<<endl;
    //vali_client_crt_path = "";
    bool is_ssl = XConfigClient::Get()->GetBool("is_ssl");
    if (is_ssl )
    {
        if (ssl_ctx_)
        {
            //����Ѿ���ssl �򲻴��������Ҫ�޸�֤���ַ����ʱ����gateway
        }
        else
        {
            LOGDEBUG("��ʼʹ��SSLͨ��");
            ssl_ctx_ = new XSSLCtx();
            ssl_ctx_->InitServer(crt_path.c_str(), key_path.c_str(), ca_path.c_str());
            //ssl_ctx_->Init(SERVER_TLS, "server.crt", "server.key", "client.crt");
            set_ssl_ctx(ssl_ctx_);
        }
    }
   return new XRouterHandle();
}
#include "xconfig_client.h"
#include "xtools.h"
#include "xlog_client.h"
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <map>
#include <thread>
#include <fstream>
using namespace std;
using namespace google;
using namespace protobuf;
using namespace compiler;
using namespace xmsg;
#define PB_ROOT "root/"

//key ip_port
static map<string, XConfig> conf_map;
static mutex conf_map_mutex;

//�洢��ǰ΢��������
static google::protobuf::Message *cur_service_conf = 0;
static mutex cur_service_conf_mutex;

//�洢��ȡ�������б�
static XConfigList *all_config = 0;
static mutex all_config_mutex;

///��ȡ���صı��ز���
int XConfigClient::GetInt(const char *key)
{
    XMutex mux(&cur_service_conf_mutex);
    if (!cur_service_conf)return 0;
    auto field = cur_service_conf->GetDescriptor()->FindFieldByName(key);
    if (!field)
    {
        return 0;
    }
    return cur_service_conf->GetReflection()->GetInt32(*cur_service_conf, field);
}
bool XConfigClient::GetBool(const char *key)
{
    XMutex mux(&cur_service_conf_mutex);
    if (!cur_service_conf) return false;
    cout<<"begin *****************************************"<<endl;
    cout<<cur_service_conf->DebugString();
    cout<<"end *****************************************"<<endl;
    //��ȡ�ֶ�
    auto field = cur_service_conf->GetDescriptor()->FindFieldByName(key);
    if (!field)
    {
        return false;
    }
    return cur_service_conf->GetReflection()->GetBool(*cur_service_conf, field);
}
std::string XConfigClient::GetString(const char *key)
{

    XMutex mux(&cur_service_conf_mutex);
    if (!cur_service_conf)return "";
    //��ȡ�ֶ�
    auto field = cur_service_conf->GetDescriptor()->FindFieldByName(key);
    if (!field)
    {
        return "";
    }
    return cur_service_conf->GetReflection()->GetString(*cur_service_conf, field);
}
//���õ�ǰ���õĶ���
void XConfigClient::SetCurServiceMessage(google::protobuf::Message *message)
{
    XMutex mux(&cur_service_conf_mutex);
    cur_service_conf = message;
}
bool XConfigClient::StartGetConf(const char *local_ip, int local_port,
    google::protobuf::Message *conf_message, ConfigTimerCBFunc func)
{
    //ע����Ϣ�ص�����
    RegMsgCallback();

    //_CRT_SECURE_NO_WARNINGS
    if (local_ip)
        strncpy(local_ip_, local_ip, 16);
    local_port_ = local_port;

    //���õ�ǰ��������
    SetCurServiceMessage(conf_message);

    this->ConfigTimerCB = func;

    this->set_timer_ms(3000);

    //��ȡ���ػ���
    stringstream ss;
    ss << local_port_ << "_conf.cache";
    ifstream ifs;
    ifs.open(ss.str(), ios::binary);
    if (!ifs.is_open())
    {
        LOGDEBUG("load local config failed!");
    }
    else
    {
        if(conf_message)
            cur_service_conf->ParseFromIstream(&ifs);
        ifs.close();
    }



    //������������������뵽�̳߳�
    StartConnect();
    return true;
}
///�����������ģ���ʼ��ʱ����ȡ����
bool XConfigClient::StartGetConf(
    const char *server_ip, int server_port,
    const char *local_ip, int local_port, 
    google::protobuf::Message *conf_message, int timeout_sec)
{
    //ע����Ϣ�ص�����
    RegMsgCallback();

    //�����������ĵ�IP�Ͷ˿�
    set_server_ip(server_ip);
    set_server_port(server_port);

    //_CRT_SECURE_NO_WARNINGS
    if(local_ip)
        strncpy(local_ip_,local_ip,16);
    local_port_ = local_port;

    //���õ�ǰ��������
    SetCurServiceMessage(conf_message);

    //������������������뵽�̳߳�
    StartConnect();


    //�ȴ������������ĳɹ� �����һ������ʧ�ܣ�����Ѷ�ʱ���ü����̳߳أ���Ҫ����Ϊ��ʱ�Զ��������������û�ȡ��Ϣ��
    if (!WaitConnected(timeout_sec))
    {
        cout << "������������ʧ��" << endl;
        return false;
    }
    if (local_port_ > 0)
        LoadConfig(local_ip_, local_port_);
    //�趨��ȡ���õĶ�ʱʱ�䣨���룩
    SetTimer(3000);

    return true;
}
bool XConfigClient::Init()
{
    XServiceClient::Init();
    
    //�ȵ���һ�ζ�ʱ����ȷ����Ϣ��ʱ��ȡ
    TimerCB();
    return true;
}
void XConfigClient::TimerCB()
{
    if (ConfigTimerCB)
        ConfigTimerCB();
    //������ȡ���õ�������Ҫ�����Զ��������⣿
    if(local_port_ > 0)
        LoadConfig(local_ip_, local_port_);
}


void XConfigClient::Wait()
{
    XThreadPool::Wait();
}


//��ȡ�������� IP���ΪNULL ��ȡ�����������ĵĵ�ַ
void XConfigClient::LoadConfig(const char *ip, int port)
{
    LOGDEBUG("��ȡ��������");
    if (port < 0 || port>65535)
    {
        LOGDEBUG("LoadConfig failed!port error");
        return;
    }
    {
        //��������һ�ε����� ����������ճɻ�ȡ���Ǿ�����
        //stringstream key;
        //key << ip << "_" << port;
        //XMutex mux(&cur_service_conf_mutex);
        //conf_map.erase(key.str());
    }
    XLoadConfigReq req;
    if(ip) //IP���ΪNULL ��ȡ�����������ĵĵ�ַ
        req.set_service_ip(ip);
    req.set_service_port(port);
    //������Ϣ�������
    SendMsg(MSG_LOAD_CONFIG_REQ, &req);
}

///��ȡ�����б��ѻ��棩�е����ã��Ḵ��һ�ݵ�out_conf
bool XConfigClient::GetConfig(const char *ip, int port, xmsg::XConfig *out_conf,int timeout_ms)
{
    //ʮ�����ж�һ��
    int count = timeout_ms / 10;
    stringstream key;
    key << ip << "_" << port;

    for (int i = 0; i < count; i++)
    {
        XMutex mutex(&conf_map_mutex);
        //��������
        auto conf = conf_map.find(key.str());
        if (conf == conf_map.end())
        {
            this_thread::sleep_for(10ms);
            continue;
        }
        //��������
        out_conf->CopyFrom(conf->second);
        return true;
    }
    LOGDEBUG("Can`t find conf");
    return false;
}

///��Ӧ��ȡ�Լ�������  �洢������
void XConfigClient::LoadConfigRes(xmsg::XMsgHead *head, XMsg *msg)
{
    LOGDEBUG("��ȡ������Ӧ");
    XConfig conf;
    if (!conf.ParseFromArray(msg->data, msg->size))
    {
        LOGDEBUG("LoadConfigRes conf.ParseFromArray failed!");
        return;
    }
    LOGDEBUG(conf.DebugString().c_str());
    //key ip_port
    stringstream key;
    key<<conf.service_ip()<<"_"<<conf.service_port();
    //��������
    conf_map_mutex.lock();
    conf_map[key.str()] = conf;
    conf_map_mutex.unlock();
   
    //û�б�������
    if (local_port_ <= 0 || !cur_service_conf)
        return;
    stringstream local_key;
    string ip = local_ip_;
    if (ip.empty())
    {
        ip = conf.service_ip();
    }
    local_key << ip << "_" << local_port_;
    if (key.str() != local_key.str())
    {
        return;
    }
    XMutex mux(&cur_service_conf_mutex);
    if (!cur_service_conf && !cur_service_conf->ParseFromString(conf.private_pb()))
    {
        return;
    }
    LOGDEBUG(cur_service_conf->DebugString().c_str());
    //�洢�������ļ�
    //�ļ��� [port]_conf.cache  20030_conf.cache
    stringstream ss;
    ss << local_port_ << "_conf.cache";
    ofstream ofs;
    ofs.open(ss.str(), ios::binary);
    if (!ofs.is_open())
    {
        LOGDEBUG("save local config failed!");
        return;
    }
    cur_service_conf->SerializePartialToOstream(&ofs);
    ofs.close();
}

//��ʾ�������﷨����
class ConfError:public MultiFileErrorCollector
{
public:
    void AddError(const std::string& filename, int line, int column,
        const std::string& message)
    {
        stringstream ss;
        ss << filename << "|" << line << "|" << column << "|" << message;
        LOGDEBUG(ss.str().c_str());
    }
};
static ConfError conf_error;
/////////////////////////////////////////////////////////////////////
//// ����proto�ļ� �̲߳���ȫ
///@para filename �ļ�·��
///@para class_name ���õ�����
Message *XConfigClient::LoadProto(std::string filename, std::string class_name, std::string &out_proto_code)
{
    //��Ҫ�ռ����� 
    delete importer_;
    importer_ = new Importer(source_tree_, &conf_error);
    if (!importer_)
    {
        return NULL;
    }
    //1 ����proto�ļ�
    string path = PB_ROOT;
    path += filename;
    //path = filename;
    //����proto�ļ�������
    auto file_desc = importer_->Import(path);
    if (!file_desc)
    {
        return NULL;
    }
    LOGDEBUG(file_desc->DebugString());
    stringstream ss;
    ss << filename << "proto �ļ����سɹ�";
    LOGDEBUG(ss.str().c_str());
    
    //��ȡ����������
    //���class_nameΪ�գ���ʹ�õ�һ������
    const Descriptor *message_desc = 0;
    if (class_name.empty())
    {
        if (file_desc->message_type_count() <= 0)
        {
            LOGDEBUG("proto�ļ���û��message");
            return NULL;
        }
        //ȡ��һ������
        message_desc = file_desc->message_type(0);
    }
    else
    {
        //���������ռ������ xmsg.XDirConfig
        string class_name_pack = "";
        //�������� �����ռ䣬�Ƿ�Ҫ�û��ṩ

        //�û�û���ṩ�����ռ� 
        if (class_name.find('.') == class_name.npos)
        {
            if (file_desc->package().empty())
            {
                class_name_pack = class_name;
            }
            else
            {
                class_name_pack = file_desc->package();
                class_name_pack += ".";
                class_name_pack += class_name;
            }
        }
        else
        {
            class_name_pack = class_name;
        }
        message_desc = importer_->pool()->FindMessageTypeByName(class_name_pack);

        if (!message_desc)
        {
            string log = "proto�ļ���û��ָ����message ";
            log += class_name_pack;
            LOGDEBUG(log.c_str());
            return NULL;
        }
    }
   
    LOGDEBUG(message_desc->DebugString());

    //��������message����

    //��̬������Ϣ���͵Ĺ������������٣����ٺ��ɴ˴�����messageҲʧЧ
    static DynamicMessageFactory factory;
    
    //����һ������ԭ��
    auto message_proto = factory.GetPrototype(message_desc);
    delete message_;
    message_ = message_proto->New();
    LOGDEBUG(message_->DebugString());
    /*
    syntax="proto3";	//�汾��
    package xmsg;		//�����ռ�
    message XDirConfig
    {
        string root = 1;
    }
    */
    
    //auto enum_type = message_desc->enum_type(0);
    //cout << enum_type->DebugString();
    //syntax="proto3";	//�汾��
    out_proto_code = "syntax=\"";
    out_proto_code += file_desc->SyntaxName(file_desc->syntax());
    out_proto_code += "\";\n";
    //package xmsg;		//�����ռ�
    out_proto_code += "package ";
    out_proto_code += file_desc->package();
    out_proto_code += ";\n";

    map<string, const EnumDescriptor*> enum_descs;
    //���������ö��
    for (int i = 0; i < message_desc->field_count(); i++)
    {
        auto field = message_desc->field(i);
        if (field->type() == FieldDescriptor::TYPE_ENUM)
        {
            if (enum_descs.find(field->enum_type()->name()) != enum_descs.end())
                continue;
            enum_descs[field->enum_type()->name()] = field->enum_type();
            out_proto_code += field->enum_type()->DebugString();
            out_proto_code += "\n";
        }

    }

    //message XDirConfig
    out_proto_code += message_desc->DebugString();
    return message_;
}

XConfigClient::XConfigClient()
{
    //�ļ�����·��
   source_tree_ = new DiskSourceTree();
   source_tree_->MapPath("", "");
   //ʹ�þ���·��ʱ������root��ʧ��
   source_tree_->MapPath(PB_ROOT, "");
}

XConfigClient::~XConfigClient()
{
}

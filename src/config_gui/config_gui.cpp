#include "config_gui.h"
#include <QMouseEvent>
#include <string>
#include <sstream>
#include "xtools.h"
#include "xconfig_client.h"
#include <QTime>
#include "config_edit.h"
#include <QMessageBox>
#include "xconfig_manager.h"
#include "xauth_client.h"
#include "xmsg_com.pb.h"
#include "login_gui..h"
using namespace xmsg;
using namespace std;
static ConfigGui *config_gui = 0;
static xmsg::XConfig config_tmp;
static mutex config_mutex;
static void  GetConfigResCB(xmsg::XConfig config)
{
    XMutex mux(&config_mutex);
    config_tmp.CopyFrom(config);
    config_gui->SEditConfCB(&config_tmp);
}
void  ConfigGui::EditConf()
{
    if (ui.tableWidget->rowCount() == 0)return;
    int row = ui.tableWidget->currentRow();
    if (row < 0)return;
    auto item_ip = ui.tableWidget->item(row, 1);
    auto item_port = ui.tableWidget->item(row, 2);
    if (!item_ip || !item_port)return;
    string ip = item_ip->text().toStdString();
    int port = atoi(item_port->text().toStdString().c_str());
    MCONF->LoadConfig(ip.c_str(), port);
}
void  ConfigGui::EditConfCB(void *config)
{
    XMutex mux(&config_mutex);
    ConfigEdit edit;
    edit.LoadConfig((XConfig*)config);
    //edit.LoadConfig(ip.c_str(), port);
    if (edit.exec() == QDialog::Accepted)
    {
        AddLog("�������óɹ�");
    }
    Refresh();
}
ConfigGui::ConfigGui(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    config_gui = this;

    //ȥ��ԭ���ڱ߿�
    setWindowFlags(Qt::FramelessWindowHint);

    //���ر���������Բ��
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    connect(this, SIGNAL(SEditConfCB(void*)), this, SLOT(EditConfCB(void*)));
    while (ui.tableWidget->rowCount() > 0)
        ui.tableWidget->removeRow(0);
    string server_ip = ui.server_ip_edit->text().toStdString();
    int server_port = ui.server_port_box->value();

    MCONF->set_server_ip(server_ip.c_str());
    MCONF->set_server_port(server_port);
    MCONF->set_auto_connect(true);

    MCONF->StartConnect();
    XAuthClient::Get()->set_server_ip(server_ip.c_str());
    XAuthClient::Get()->set_server_port(server_port);
    //�����Զ�����
    XAuthClient::Get()->set_auto_connect(true);
    XAuthClient::Get()->StartConnect();

    //while (!XAuthClient::Get()->is_connected())
    //{
    //    cout << "-" << flush;
    //    //cout << XAuthClient::Get()->is_connecting() << endl; 
    //    //cout << XAuthClient::Get()->is_connected() << endl; 
    //    //cout << "." << flush;
    //    this_thread::sleep_for(100ms);
    //}
  /*  XAuthClient::Get()->LoginReq("root", "123456");
    this_thread::sleep_for(100ms);
    XLoginRes login;
    if (XAuthClient::Get()->GetLoginInfo("root", &login, 500))
    {
        cout << "login success!" << endl;
    }
    else
    {
        cout << "login failed!" << endl;
    }
    MCONF->set_login(login);*/
    MCONF->LoadConfigResCB = GetConfigResCB;
    Refresh();

}
//��ʾ����־�б���
void ConfigGui::AddLog(const char *log)
{
    //����������ʾ
    auto t = QTime::currentTime().toString("HH:mm:ss");
    QString str = t;
    str += " ";
    str += QString::fromLocal8Bit(log);
    LOGDEBUG(log);
    ui.log_list_Widget->insertItem(0, new QListWidgetItem(str));
}


//ɾ��ѡ�е�����
void ConfigGui::DelConf()
{
    if (ui.tableWidget->rowCount() == 0)return;
    int row = ui.tableWidget->currentRow();
    if (row < 0)return;
    auto item_name = ui.tableWidget->item(row, 0);
    auto item_ip = ui.tableWidget->item(row, 1);
    auto item_port = ui.tableWidget->item(row, 2);
    string name = item_name->text().toStdString();
    string ip = item_ip->text().toStdString();
    int port = atoi(item_port->text().toStdString().c_str());
    
    stringstream ss;
    ss << "��ȷ��ɾ��" << name << "|" << ip << ":" << port << " ΢����������";
    if (QMessageBox::information(0, "",
        QString::fromLocal8Bit(ss.str().c_str()),
        QMessageBox::Yes|QMessageBox::No
        ) == QMessageBox::No)
    {
        return;
    }
    MCONF->DeleteConfig(ip.c_str(), port);
    ss.clear();
    ss << "ɾ������" << name << "|" << ip << ":" << port;
    AddLog(ss.str().c_str());
    //��ȡѡ�е�����name IP port 
    Refresh();
}
//��������
void ConfigGui::AddConf()
{
    //��ģ̬���ڣ��ȴ��˳�
    ConfigEdit edit;
    if (edit.exec() == QDialog::Accepted)
    {
        AddLog("�������óɹ�");
    }
    Refresh();
}
bool ConfigGui::CheckLogin(std::string ip, int port)
{
    //��֤��¼�Ƿ���Ч���Ƿ�ʱ
    //��ʱǰһ���ӷ���token��ʱ����
    static bool is_login = false;
    static std::string last_ip = "";
    static int last_port = 0;
    //Ҫ���Ǹ���������������µ�¼
    if (is_login && ip == last_ip&& last_port== port)
        return true;
    last_ip = ip;
    last_port = port;
    is_login = false;
    XAUTH->set_server_ip(ip.c_str());
    XAUTH->set_server_port(port);
    XAUTH->Close();
    if (!XAUTH->AutoConnect(1))
    {
        AddLog("��֤��������ʧ��");
        return false;
    }

    LoginGUI login;
    if (login.exec() == QDialog::Rejected)
    {
        return false;
    }
    is_login = true;
    return true;
}

//ˢ����ʾ�����б�
void ConfigGui::Refresh()
{
    while (ui.tableWidget->rowCount() > 0)
        ui.tableWidget->removeRow(0);
    //�Ͽ�����������޸��������ĵ�IP���߶˿�
    string server_ip = ui.server_ip_edit->text().toStdString();
    int server_port = ui.server_port_box->value();

    if (!CheckLogin(server_ip, server_port))
    {
        AddLog("��֤��¼ʧ��");
        return;
    }
    //������ʷ�б�
    AddLog("������ʷ�б�");
    while (ui.tableWidget->rowCount() > 0)
        ui.tableWidget->removeRow(0);

    stringstream ss;
    ss << server_ip << ":" << server_port;
    LOGDEBUG(ss.str().c_str());
    
    //�ر�֮ǰ�����ӣ����½�������
    MCONF->set_server_ip(server_ip.c_str());
    MCONF->set_server_port(server_port);
    MCONF->set_auto_delete(false);
    
    MCONF->Close();
    if (!MCONF->AutoConnect(1))
    {
    //LOGDEBUG("������������ʧ��!");
        AddLog("������������ʧ��");
        return;
    }
    //LOGDEBUG("�����������ĳɹ�!");
    AddLog("�����������ĳɹ�");



    //���������Ļ�ȡ�����б�
    auto confs = MCONF->GetAllConfig(1, 10000, 2);
    LOGDEBUG(confs.DebugString());

    //�����ȡ���б�
    ui.tableWidget->setRowCount(confs.config_size());
    for (int i = 0; i < confs.config_size(); i++)
    {
        auto conf = confs.config(i);
        ui.tableWidget->setItem(i, 0, new QTableWidgetItem(conf.service_name().c_str()));
        ui.tableWidget->setItem(i, 1, new QTableWidgetItem(conf.service_ip().c_str()));
        stringstream ss;
        ss << conf.service_port();
        ui.tableWidget->setItem(i, 2, new QTableWidgetItem(ss.str().c_str()));
    }
    AddLog("���������б����");
}

static bool mouse_press = false;
static QPoint mouse_point;
void ConfigGui::mouseMoveEvent(QMouseEvent *ev)
{
    //û�а��£�����ԭ�¼�
    if (!mouse_press)
    {
        QWidget::mouseMoveEvent(ev);
        return;
    }
    auto cur_pos = ev->globalPos();
    this->move(cur_pos - mouse_point);
}
void ConfigGui::mousePressEvent(QMouseEvent *ev)
{
    //���������¼�¼λ��
    if (ev->button() == Qt::LeftButton)
    {
        mouse_press = true;
        mouse_point = ev->pos();
    }

}
void ConfigGui::mouseReleaseEvent(QMouseEvent *ev)
{
    mouse_press = false;
}
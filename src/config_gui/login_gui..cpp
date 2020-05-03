#include "login_gui..h"
#include <QMessageBox>
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
void LoginGUI::Login()
{
    if (ui.usernameEdit->text().isEmpty())
    {
        QMessageBox::information(this, "", QString::fromLocal8Bit("�û�������Ϊ��"));
        return;
    }
    username_ = ui.usernameEdit->text().toStdString();
    if (ui.passwordEdit->text().isEmpty())
    {
        QMessageBox::information(this, "", QString::fromLocal8Bit("���벻��Ϊ��"));
        return;
    }
    password_ = ui.passwordEdit->text().toStdString();



    XAUTH->LoginReq(username_, password_);
    this_thread::sleep_for(500ms);
    XLoginRes login;
    if (!XAuthClient::Get()->GetLoginInfo(username_, &login, 1000))
    {
        QMessageBox::information(this, "", QString::fromLocal8Bit("�û��������������"));
        return;
    }

    cout << "login success!" << endl;
    MCONF->set_login(login);
    accept();
}
LoginGUI::LoginGUI(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

LoginGUI::~LoginGUI()
{
}

#include "config_gui.h"
#include "xconfig_client.h"
#include "config_edit.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    //��ʼ�������ÿͻ��ˣ������̳߳�
   //XConfigClient::Get()->StartGetConf("127.0.0.1", CONFIG_PORT, 0, 0, 0);
    QApplication a(argc, argv);
    //ConfigEdit edit;
    //edit.exec();
   // return 0;
    ConfigGui w;
    w.show();
    return a.exec();
}

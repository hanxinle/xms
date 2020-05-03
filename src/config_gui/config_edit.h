#pragma once

#include <QDialog>
#include "ui_config_edit.h"
namespace google
{
    namespace protobuf
    {
        class Message;
    }
}
namespace xmsg
{
    class XConfig;
}
class ConfigEdit : public QDialog
{
    Q_OBJECT

public:
    ConfigEdit(QWidget *parent = Q_NULLPTR);
    ~ConfigEdit();
    //bool LoadConfig(const char *ip,int port);
    bool LoadConfig(xmsg::XConfig *config);
    void LoadProto(const char *filename, const char *class_name);
    void InitGui(); 
signals:
    void AddLog(const char *log);
    //��Ϣ�ص�
    void MessageCBSig(bool is_ok, const char *msg);
public slots:
    void Save();
    ///ѡ��proto�ļ��������ض�̬����
    void LoadProto();


    //��Ϣ�ص�
    void MessageCB(bool is_ok, const char *msg);
private:
    Ui::ConfigEdit ui;
    //����������Ϣ������������Щ�Ǹ���proto�ļ����ɵ�
    int config_row_count_ = 0;
    //���ڴ洢������
    google::protobuf::Message *message_ = 0;
    xmsg::XConfig *config_ = 0;

};

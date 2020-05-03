#pragma once

#include <QtWidgets/QWidget>
#include "ui_config_gui.h"

class ConfigGui : public QWidget
{
    Q_OBJECT

public:
    ConfigGui(QWidget *parent = Q_NULLPTR);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    void SEditConfCB(void *config);
public slots:
    //ˢ����ʾ�����б�
    void Refresh();

    //��������
    void AddConf();

    //ɾ��ѡ�е�����
    void DelConf();

    //�༭����
    void EditConf();

    void EditConfCB(void *config);

    //��ʾ����־�б���
    void AddLog(const char *log);
private:
    Ui::ConfigGuiClass ui;

    bool CheckLogin(std::string ip,int port);

};

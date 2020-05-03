#include "config_edit.h"
#include "xtools.h"
#include <string>
#include <QFileDialog>
#include <QComboBox>
#include <QMessageBox>
#include "xconfig_client.h"
#include "xconfig_manager.h"
#include <fstream>
using namespace std;
using namespace xmsg;
static ConfigEdit *cur_edit = 0;
static void ConfigMessageCB(bool is_ok, const char *msg)
{
    if (cur_edit)
        cur_edit->MessageCBSig(is_ok, msg);
}
ConfigEdit::ConfigEdit(QWidget *parent)
    : QDialog(parent)
{
    cur_edit = this;
    ui.setupUi(this);
    setLayout(ui.formLayout);
    QObject::connect(this,
        SIGNAL(MessageCBSig(bool, const char *)),
        this,
        SLOT(MessageCB(bool, const char *))
    );
    MCONF->SendConfigResCB = ConfigMessageCB;
    config_row_count_ = ui.formLayout->rowCount();
}

ConfigEdit::~ConfigEdit()
{
    cur_edit = NULL;
    delete config_;
    config_ = NULL;
}

//��Ϣ�ص�
void ConfigEdit::MessageCB(bool is_ok, const char *msg)
{
    if (!is_ok)
    {
        QMessageBox::information(this, "", msg);
        return;
    }
    accept();
}

void ConfigEdit::InitGui()
{
    if (config_)
    {
        ui.proto_textEdit->setText(config_->proto().c_str());
        ui.service_ipLineEdit->setText(config_->service_ip().c_str());
        ui.service_nameLineEdit->setText(config_->service_name().c_str());
        ui.service_portSpinBox->setValue(config_->service_port());
    }

    if (!message_) return;
    ui.proto_nameEdit->setText(message_->GetTypeName().c_str());
    LOGDEBUG(message_->DebugString());

    //����message �������������������
    //��ȡ��������
    auto desc = message_->GetDescriptor();
    auto ref = message_->GetReflection();
    //�����ֶ�
    int field_count = desc->field_count();
    for (int i = 0; i < field_count; i++)
    {
        //�����ֶ�����
        auto field = desc->field(i);
        auto type = field->type();

        //��������
        QSpinBox *int_box = 0;

        //���븡��
        QDoubleSpinBox  *double_box = 0;

        //�ַ��� byte
        QLineEdit *str_edit = 0;

        //bool ö��
        QComboBox *combo_box = 0;

        string tmp = "";
        //֧�����֣��ַ�����ö��
        switch (type)
        {
            //֧����������
        case google::protobuf::FieldDescriptor::TYPE_INT64:
            int_box = new QSpinBox();
            int_box->setMaximum(INT32_MAX);
            ui.formLayout->addRow(field->name().c_str(), int_box);
            int_box->setValue(ref->GetInt64(*message_, field));
            break;
        case google::protobuf::FieldDescriptor::TYPE_INT32:
            //case google::protobuf::FieldDescriptor::TYPE_UINT64:
            //case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            //case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            //case google::protobuf::FieldDescriptor::TYPE_UINT32:
            //case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            //case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            //case google::protobuf::FieldDescriptor::TYPE_SINT32:
            //case google::protobuf::FieldDescriptor::TYPE_SINT64:
            int_box = new QSpinBox();
            int_box->setMaximum(INT32_MAX);
            ui.formLayout->addRow(field->name().c_str(), int_box);
            int_box->setValue(ref->GetInt32(*message_, field));

            break;
            //֧�ָ�����
        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            double_box = new QDoubleSpinBox();
            double_box->setMaximum(FLT_MAX);
            ui.formLayout->addRow(field->name().c_str(), double_box);
            double_box->setValue(ref->GetDouble(*message_, field));
            break;
        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            double_box = new QDoubleSpinBox();
            double_box->setMaximum(FLT_MAX);
            ui.formLayout->addRow(field->name().c_str(), double_box);
            double_box->setValue(ref->GetFloat(*message_, field));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
        case google::protobuf::FieldDescriptor::TYPE_STRING:
            str_edit = new QLineEdit();
            ui.formLayout->addRow(field->name().c_str(), str_edit);
            tmp = ref->GetString(*message_, field);
            str_edit->setText(ref->GetString(*message_, field).c_str());
            break;
        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            combo_box = new QComboBox();
            combo_box->addItem("true", true);
            combo_box->addItem("false", false);
            ui.formLayout->addRow(field->name().c_str(), combo_box);
            if (ref->GetBool(*message_, field))
                combo_box->setCurrentIndex(0);
            else
                combo_box->setCurrentIndex(1);
            break;
        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            combo_box = new QComboBox();
            for (int j = 0; j < field->enum_type()->value_count(); ++j)
            {
                string enum_name = field->enum_type()->value(j)->name();
                int enum_val = field->enum_type()->value(j)->number();
                combo_box->addItem(enum_name.c_str(), enum_val);
            }
            ui.formLayout->addRow(field->name().c_str(), combo_box);
            combo_box->setCurrentIndex(combo_box->findData(ref->GetEnumValue(*message_, field)));
            break;
        default:
            break;
        }
    }
}

void ConfigEdit::LoadProto(const char *filename, const char *class_name)
{
    //����֮ǰ��������Ŀ
    while (ui.formLayout->rowCount() != config_row_count_)
        ui.formLayout->removeRow(config_row_count_);
    if (!filename)
        return;
    //if (config_)
    //    delete config_;
    //config_ = NULL;
    //delete message_;
    //message_ = NULL;

    //�û������������ƣ����û�����ƣ���ʹ��proto�ļ��еĵ�һ������
    string class_name_str = "";
    if (class_name)
    {
        class_name_str = class_name;
    }

    string proto_code = "";
    message_ = XConfigClient::Get()->LoadProto(filename, class_name_str, proto_code);
    if (message_ == NULL)
    {
        LOGDEBUG("XConfigClient::Get()->LoadProto failed!");
        return;
    }
}
bool ConfigEdit::LoadConfig(xmsg::XConfig *config)
{
    if (!config)return false;
    if (!config_)
        config_ = new XConfig();
    config_->CopyFrom(*config);
    //const char *filename, const char *class_name
    string filename = "tmp.proto";
    ofstream ofs(filename);
    ofs << config_->proto();
    ofs.close();
    LoadProto(filename.c_str(), 0);
    InitGui();
    return true;
}
//
//bool ConfigEdit::LoadConfig(const char *ip, int port)
//{
//    MCONF->LoadConfig(ip, port);
//    //XConfig conf;
//    if (!config_)
//        config_ = new XConfig();
//    bool is_find = false;
//    for (int i = 0; i < 100; i++)
//    {
//        if (MCONF->GetConfig(ip, port, config_))
//        {
//            is_find = true;
//            break;
//        }
//        this_thread::sleep_for(10ms);
//    }
//    if (!is_find)return false;
//
//    //const char *filename, const char *class_name
//    string filename = "tmp.proto";
//    ofstream ofs(filename);
//    ofs << config_->proto();
//    ofs.close();
//    LoadProto(filename.c_str(), 0);
//
//    cout << "====================================================\n";
//    if (this->message_)
//    {
//        this->message_->ParseFromString(config_->private_pb());
//        cout << this->message_->DebugString() << endl;
//        cout << "-----------------------------------------------------\n";
//    }
//    cout << this->config_->DebugString() << endl;
//    cout << "====================================================\n";
//    InitGui();
//    return true;
//}
void ConfigEdit::Save()
{
    if (!message_)
    {
        LOGDEBUG("save failed! message is null");
        QMessageBox::information(this, "", "proto is empty");
        return;
    }

    if (ui.service_nameLineEdit->text().isEmpty()
        || ui.service_ipLineEdit->text().isEmpty()
        || ui.proto_textEdit->toPlainText().isEmpty())
    {
        QMessageBox::information(this, "", "service_name,service_ip,proto is empty");
        return;
    }

    //���������� �洢��message��

    //������Ϣ
    //��������
    //��������
    auto desc = message_->GetDescriptor();
    //message ����
    auto ref = message_->GetReflection();

    for (int i = this->config_row_count_; i < ui.formLayout->rowCount(); i++)
    {
        //////////////////////////////////////////////
        ///�ҵ�key label��text
        auto label_item = ui.formLayout->itemAt(i, QFormLayout::LabelRole);
        if (!label_item)continue;
        //����ʱת����ʧ�ܷ���NULL
        auto label = dynamic_cast<QLabel*>(label_item->widget());
        if (!label)continue;
        auto field_name = label->text().toStdString();

        //////////////////////////////////////////////
        //��ȡvalue ��ȡ����ؼ��е�ֵ ö�١����Ρ����㡢�ַ���
        // ��ȡ�ؼ�
        auto field_item = ui.formLayout->itemAt(i, QFormLayout::FieldRole);
        if (!field_item)continue;
        auto field_edit = field_item->widget();
        //��ȡ�ֶ����������ͣ�
        auto field_desc = desc->FindFieldByName(field_name);
        if (!field_desc)continue;
        auto type = field_desc->type();

        //��ȡ�ؼ���ֵ�����õ�message
                //��������
        QSpinBox *int_box = 0;
        //���븡��
        QDoubleSpinBox  *double_box = 0;
        //�ַ��� byte
        QLineEdit *str_edit = 0;
        //bool ö��
        QComboBox *combo_box = 0;

        switch (type)
        {
            //֧����������
        case google::protobuf::FieldDescriptor::TYPE_INT64:
            int_box = dynamic_cast<QSpinBox*>(field_edit);
            if (!int_box)continue;
            ref->SetInt64(message_, field_desc, int_box->value());
            break;
        case google::protobuf::FieldDescriptor::TYPE_INT32:
            int_box = dynamic_cast<QSpinBox*>(field_edit);
            if (!int_box)continue;
            ref->SetInt32(message_, field_desc, int_box->value());
            break;
            //֧�ָ�����
        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            double_box = dynamic_cast<QDoubleSpinBox*>(field_edit);
            if (!double_box)continue;
            ref->SetDouble(message_, field_desc, double_box->value());
            break;
        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            double_box = dynamic_cast<QDoubleSpinBox*>(field_edit);
            if (!double_box)continue;
            ref->SetFloat(message_, field_desc, double_box->value());
            break;
        case google::protobuf::FieldDescriptor::TYPE_BYTES:
        case google::protobuf::FieldDescriptor::TYPE_STRING:
            str_edit = dynamic_cast<QLineEdit*>(field_edit);
            if (!str_edit)continue;
            ref->SetString(message_, field_desc, str_edit->text().toStdString());
            break;
        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            combo_box = dynamic_cast<QComboBox*>(field_edit);
            if (!combo_box)continue;
            ref->SetBool(message_, field_desc, combo_box->currentData().toBool());
            break;
        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            combo_box = dynamic_cast<QComboBox*>(field_edit);
            if (!combo_box)continue;
            ref->SetEnumValue(message_, field_desc, combo_box->currentData().toInt());
            break;
        default:
            break;
        }
    }
    //�������� ���ֻ�����Ϣ��������Ϣ
    XConfig config;
    //������Ϣ
    config.set_service_name(ui.service_nameLineEdit->text().toStdString());
    config.set_service_ip(ui.service_ipLineEdit->text().toStdString());
    config.set_service_port(ui.service_portSpinBox->value());
    config.set_proto(ui.proto_textEdit->toPlainText().toStdString());
    //���л�message
    string msg_pb = message_->SerializeAsString();
    config.set_private_pb(msg_pb);

    LOGDEBUG(message_->DebugString());
    LOGDEBUG(config.DebugString());
    //�洢���õ���������
    MCONF->SendConfig(&config);
}

///ѡ��proto�ļ��������ض�̬����
void ConfigEdit::LoadProto()
{
    LOGDEBUG("LoadProto");
    //����֮ǰ��������Ŀ
    while (ui.formLayout->rowCount() != config_row_count_)
        ui.formLayout->removeRow(config_row_count_);

    //delete message_;
    //message_ = NULL;

    //�û������������ƣ����û�����ƣ���ʹ��proto�ļ��еĵ�һ������
    QString class_name = ui.proto_nameEdit->text();
    string class_name_str = "";
    if (!class_name.isEmpty())
    {
        class_name_str = class_name.toStdString();
    }

    //�û�ѡ��proto�ļ�
    QString filename = QFileDialog::getOpenFileName(this,
        QString::fromLocal8Bit("��ѡ��proto�ļ�"), "", "*.proto");
    if (filename.isEmpty())
        return;
    LOGDEBUG(filename.toStdString().c_str());
    string proto_code = "";
    message_ = XConfigClient::Get()->LoadProto(filename.toStdString(), class_name_str, proto_code);
    if (message_ == NULL)
    {
        LOGDEBUG("XConfigClient::Get()->LoadProto failed!");
        return;
    }
    ui.proto_nameEdit->setText(message_->GetTypeName().c_str());
    ui.proto_textEdit->setText(proto_code.c_str());

    if (config_)
        delete config_;
    config_ = NULL;

    LoadProto(filename.toStdString().c_str(), class_name_str.c_str());
    InitGui();
    ////����message �������������������
    ////��ȡ��������
    //auto desc = message_->GetDescriptor();
    ////�����ֶ�
    //int field_count = desc->field_count();
    //for (int i = 0; i < field_count; i++)
    //{
    //    //�����ֶ�����
    //    auto field = desc->field(i);
    //    auto type = field->type();


    //    //��������
    //    QSpinBox *int_box = 0;

    //    //���븡��
    //    QDoubleSpinBox  *double_box = 0;

    //    //�ַ��� byte
    //    QLineEdit *str_edit = 0;

    //    //bool ö��
    //    QComboBox *combo_box = 0;


    //    //֧�����֣��ַ�����ö��
    //    switch (type)
    //    {
    //        //֧����������
    //    case google::protobuf::FieldDescriptor::TYPE_INT64:
    //    case google::protobuf::FieldDescriptor::TYPE_INT32:
    //    //case google::protobuf::FieldDescriptor::TYPE_UINT64:
    //    //case google::protobuf::FieldDescriptor::TYPE_FIXED64:
    //    //case google::protobuf::FieldDescriptor::TYPE_FIXED32:
    //    //case google::protobuf::FieldDescriptor::TYPE_UINT32:
    //    //case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
    //    //case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
    //    //case google::protobuf::FieldDescriptor::TYPE_SINT32:
    //    //case google::protobuf::FieldDescriptor::TYPE_SINT64:
    //        int_box = new QSpinBox();
    //        int_box->setMaximum(INT32_MAX);
    //        ui.formLayout->addRow(field->name().c_str(), int_box);

    //        break;
    //        //֧�ָ�����
    //    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    //    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    //        double_box = new QDoubleSpinBox();
    //        double_box->setMaximum(FLT_MAX);
    //        ui.formLayout->addRow(field->name().c_str(), double_box);
    //        break;

    //    case google::protobuf::FieldDescriptor::TYPE_BYTES:
    //    case google::protobuf::FieldDescriptor::TYPE_STRING:
    //        str_edit = new QLineEdit();
    //        ui.formLayout->addRow(field->name().c_str(), str_edit);
    //        break;
    //    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    //        combo_box = new QComboBox();
    //        combo_box->addItem("true", true);
    //        combo_box->addItem("false", true);
    //        ui.formLayout->addRow(field->name().c_str(), combo_box);
    //        break;
    //    case google::protobuf::FieldDescriptor::TYPE_ENUM:
    //        combo_box = new QComboBox();
    //        for (int j = 0; j < field->enum_type()->value_count(); ++j)
    //        {
    //            string enum_name = field->enum_type()->value(j)->name();
    //            int enum_val = field->enum_type()->value(j)->number();
    //            combo_box->addItem(enum_name.c_str(), enum_val);
    //        }
    //        ui.formLayout->addRow(field->name().c_str(), combo_box);
    //        break;

    //    case google::protobuf::FieldDescriptor::TYPE_GROUP:
    //        break;
    //    case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
    //        break;

    //    default:
    //        break;
    //    }

    //}


}
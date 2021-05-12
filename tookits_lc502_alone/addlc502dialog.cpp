#include "addlc502dialog.h"
#include "ui_addlc502dialog.h"
#include <QMessageBox>
#include <QRegExpValidator>
#include <QDebug>
#include <cast/cast.h>
#include "Common.h"

namespace cast = qingpei::toolkit::cast;

CAddLC502Dialog::CAddLC502Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAddLC502Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("添加LC502"));
    m_is_add_item = true;
    //ip输入格式校验
    QRegExp ip_exp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator *ip_val = new QRegExpValidator(ip_exp,this);
    ui->add_ip_lineEdit->setValidator(ip_val);

    //port输入格式校验
    QRegExp port_exp("(^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$))");
    QRegExpValidator *port_val = new QRegExpValidator(port_exp,this);
    ui->add_udpPort_lineEdit->setValidator(port_val);
    ui->add_udpPort_lineEdit->setText("3434");

    //mac输入格式校验
//    QRegExp mac_exp("(([0-9]|[A-F]|[a-f]){16}$)");
//    QRegExpValidator *mac_val = new QRegExpValidator(mac_exp,this);
//    ui->add_mac_lineEdit->setValidator(mac_val);

    m_name = "控制器";
    m_name_number = 1;
    m_ip = "127.0.0.1";
//    m_mac_addr = "AA11223344556677";

    this->setWindowModality(Qt::ApplicationModal);
}

CAddLC502Dialog::~CAddLC502Dialog()
{
    delete ui;
}

void CAddLC502Dialog::ShowDlg()
{
//    ui->add_name_lineEdit->setText(m_name + QString::number(m_name_number));
//    ui->add_ip_lineEdit->setText(m_ip);
//    ui->add_mac_lineEdit->setText(m_mac_addr);
//    ui->get_mac_pushButton->setEnabled(true);
    this->show();
}

void CAddLC502Dialog::SetEditMsg(QString name,QString ip,QString mask, QString gateway, QString port,QString loginName)
{
    ui->add_name_lineEdit->setText(name);
    ui->add_ip_lineEdit->setText(ip);
    ui->add_mask_lineEdit->setText(mask);
    ui->add_gateway_lineEdit->setText(gateway);
    ui->add_udpPort_lineEdit->setText(port);
    ui->add_loginUser_lineEdit->setText(loginName);
    this->show();
}

void CAddLC502Dialog::on_add_ok_pushButton_clicked()
{
    //添加LC502列表
    if(ui->add_ip_lineEdit->text().isEmpty())
    {
        SetMessageBox(QMessageBox::Warning,"警告","IP不能为空");
        return;
    }
    if(ui->add_mask_lineEdit->text().isEmpty())
    {
        SetMessageBox(QMessageBox::Warning,"警告","子网掩码不能为空");
        return;
    }
    if(ui->add_gateway_lineEdit->text().isEmpty())
    {
        SetMessageBox(QMessageBox::Warning,"警告","网关不能为空");
        return;
    }
    if(ui->add_udpPort_lineEdit->text().isEmpty())
    {
        SetMessageBox(QMessageBox::Warning,"警告","udp端口号不能为空");
        return;
    }
    if(ui->add_loginUser_lineEdit->text().isEmpty())
    {
        SetMessageBox(QMessageBox::Warning,"警告","登录用户名不能为空");
        return;
    }
    //ip合法性校验
    QString ip = ui->add_ip_lineEdit->text();
    int pos = 0;
    if(ui->add_ip_lineEdit->validator()->validate(ip,pos) != QValidator::Acceptable)
    {
        SetMessageBox(QMessageBox::Warning,"警告","IP不符合规则");
        return;
    }
    //port合法性校验
    QString port = ui->add_udpPort_lineEdit->text();
    int port_pos = 0;
    if(ui->add_udpPort_lineEdit->validator()->validate(port,port_pos) != QValidator::Acceptable)
    {
        SetMessageBox(QMessageBox::Warning,"警告","port不符合规则");
        return;
    }    
//    _AutoCompleteMac();
    emit AddLC502MsgSig(m_is_add_item,
                        ui->add_name_lineEdit->text(),
                        ui->add_ip_lineEdit->text(),
                        ui->add_mask_lineEdit->text(),
                        ui->add_gateway_lineEdit->text(),
                        ui->add_udpPort_lineEdit->text(),
                        ui->add_loginUser_lineEdit->text());
}

void CAddLC502Dialog::on_add_cancel_pushButton_clicked()
{
    this->close();
}

//void CAddLC502Dialog::on_get_mac_pushButton_clicked()
//{
//    this->setEnabled(false);
//    emit GetControllerMacAddrSig(ui->add_ip_lineEdit->text(),ui->add_port_lineEdit->text().toInt());
//}

//void CAddLC502Dialog::OnGetControllerMacAddr(QString mac_addr)
//{
//    this->setEnabled(true);
//    if(mac_addr != "")
//        ui->add_mac_lineEdit->setText(mac_addr);
//    else
//        SetMessageBox(QMessageBox::Warning,"错误","获取MAC地址失败");
//}

void CAddLC502Dialog::OnControllerExsited(bool is_existed)
{
    if(is_existed){
        SetMessageBox(QMessageBox::Warning,"警告","该LC502已存在");
        this->close();
    }else{
        SetNameIpPortAddr(ui->add_name_lineEdit->text(),ui->add_udpPort_lineEdit->text(),ui->add_ip_lineEdit->text());
        this->close();
    }
}

//void CAddLC502Dialog::_AutoCompleteMac()
//{
//    QString mac_addr = ui->add_mac_lineEdit->text();
//    int complete_num = 16 - mac_addr.length();
//    for (int i = 0; i < complete_num; i++) {
//        mac_addr.insert(0,'0');
//    }
//    ui->add_mac_lineEdit->setText(mac_addr);
//}

void CAddLC502Dialog::SetNameIpPortAddr(QString name,QString port,QString ip)
{
   _SetNameNumber(name);
//   _SetMacAddr(mac);
   _SetUdpPort(port);
   _SetIp(ip);
}

void CAddLC502Dialog::_SetNameNumber(QString name)
{
    int length = name.length();
    int index = 0;
    if(length > 0 && name[length - 1] >= '0' && name[length - 1] <= '9')
    {
        index = length - 1;
        for (int i = length - 2; i >= 0; i--) {
            if(name[i] >= '0' && name[i] <= '9')
                index = i;
            else
                break;
        }
        m_name = name;      
        m_name.remove(index,length - index);        
        m_name_number = name.remove(0,index).toInt() + 1;
    }
    else{
        if(length == 0)
            name = "控制器";
        m_name = name;
        m_name_number = 1;
    }
}


//void CAddLC502Dialog::_SetMacAddr(QString mac_addr)
//{
//    int length = mac_addr.length();
//    if(length != ui->add_mac_lineEdit->maxLength() || ui->add_mac_lineEdit->text() == "FFFFFFFFFFFFFFFF")
//        return;
//    uint8_t mac_data[8] = {0};
//    std::string mac_str = mac_addr.toStdString();
//    hex_string_to_bytes(mac_str,mac_data);
//    uint64_t mac = qingpei::toolkit::cast::bytes_to_num<int64_t>(mac_data);
//    mac++;
//    m_mac_addr = QString::fromStdString(qingpei::toolkit::cast::num_to_hex_string(mac));
//}

void CAddLC502Dialog::_SetUdpPort(QString port)
{
    int p = port.toInt();
    if(p>65535)
        return;
    else
        p++;
    m_udpPort = QString("%1").arg(p);
}

void CAddLC502Dialog::_SetIp(QString ip)
{
    QStringList ip_list = ip.split(".");
    if(ip_list.size() < 4)
        return;
    int ip_0 = ip_list.at(0).toInt();
    int ip_1 = ip_list.at(1).toInt();
    int ip_2 = ip_list.at(2).toInt();
    int ip_3 = ip_list.at(3).toInt();
    if(ip_3 >= 255)
    {
        if(ip_2 >= 255)
        {
            if(ip_1 >= 255)
            {
                if(ip_0 >= 255)
                    return;
                else
                    ip_0++;
            }
            else
                ip_1 ++;
        }
        else
            ip_2++;
    }
    else
        ip_3++;

    m_ip =QString("%1.%2.%3.%4").arg(ip_0).arg(ip_1).arg(ip_2).arg(ip_3);
}

QString CAddLC502Dialog::GetName()
{
    QString name = m_name + QString::number(m_name_number);
    m_name_number++;
    return name;
}

//void CAddLC502Dialog::on_add_mac_lineEdit_textEdited(const QString &arg1)
//{
//    ui->add_mac_lineEdit->setText(arg1.toUpper());
//}

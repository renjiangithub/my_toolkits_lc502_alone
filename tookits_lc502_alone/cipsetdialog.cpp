#include "cipsetdialog.h"
#include "ui_cipsetdialog.h"
#include "mystruct.h"

#include <QHostAddress>
#include <QNetworkInterface>

CIpSetDialog::CIpSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CIpSetDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("搜索设置");
    ui->ip_comboBox->addItem(g_sudp_property.m_broadcast_use_ip);
}

CIpSetDialog::~CIpSetDialog()
{
    delete ui;
}

void CIpSetDialog::ShowDlg()
{
    this->show();
}

void CIpSetDialog::_SearchAllLocalIp()
{
    ui->ip_comboBox->clear();
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol()==QAbstractSocket::IPv4Protocol
                && address.toString()!="127.0.0.1"){
            ui->ip_comboBox->addItem(address.toString());
        }
    }
    ui->ip_comboBox->setCurrentIndex(0);
}

void CIpSetDialog::on_requery_btn_clicked()
{
    _SearchAllLocalIp();
}

void CIpSetDialog::on_ok_btn_clicked()
{
    g_sudp_property.m_broadcast_use_ip=ui->ip_comboBox->currentText();
    this->close();
}

void CIpSetDialog::on_cancel_btn_clicked()
{
    this->close();
}

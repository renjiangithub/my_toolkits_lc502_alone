#ifndef ADDLC502DIALOG_H
#define ADDLC502DIALOG_H

#include <QDialog>

namespace Ui {
class CAddLC502Dialog;
}

class CAddLC502Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAddLC502Dialog(QWidget *parent = nullptr);
    ~CAddLC502Dialog();
    void ShowDlg();
    void SetEditMsg(QString name,QString ip,QString mask, QString gateway, QString port,QString loginName);
    void SetNameIpPortAddr(QString name, QString port, QString ip);
    QString GetName();
private:
//    void _AutoCompleteMac();
    void _SetNameNumber(QString name);
//    void _SetMacAddr(QString mac_addr);
    void _SetUdpPort(QString port);
    void _SetIp(QString ip);

public slots:
    void on_add_ok_pushButton_clicked();

    void on_add_cancel_pushButton_clicked();

//    void on_get_mac_pushButton_clicked();

//    void OnGetControllerMacAddr(QString mac_addr);

    void OnControllerExsited(bool is_existed);

signals:
    void AddLC502MsgSig(bool is_add_item,QString name,QString ip,QString mask,QString gateway, QString port,QString loginUser);
//    void GetControllerMacAddrSig(QString ip, int port);

public:
    bool m_is_add_item;
    QString m_name;
    int m_name_number;
    QString m_ip;
    QString m_udpPort;
//    QString m_mac_addr;


private slots:
//    void on_add_mac_lineEdit_textEdited(const QString &arg1);

private:
    Ui::CAddLC502Dialog *ui;

};

#endif // ADDLC502DIALOG_H

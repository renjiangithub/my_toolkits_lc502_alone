#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "addLC502Dialog.h"
#include "lc502controller.h"
#include "cipsetdialog.h"
#include "udpthread.h"

#include <QMainWindow>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_add_lc502_btn_clicked();
    void _OnLC502ListChanged(bool is_add_item,QString name,QString ip,QString mask,QString gateway, QString port,QString loginName);
    void on_ip_set_action_triggered();

    void _OnSendLC502UdpReadData(QVector<CUdpDataFrame> udp_data_frame);

    void _OnDelLc502BtnClicked();
    void _OnEditBtnClicked();
    void on_button_rs485_query_clicked();

private:
    void _InitMainWindow();
    void _InitDialog();
    void _LoadConfig();
    void _LoadLC502(QSettings &read_ini);
    void _LoadLog(QSettings &read_ini);
    void _AddLC502InList(QString name,QString ip,QString mask,QString gateway,int port,QString loginName);
    void _OnAddLC502Item(QString name,QString ip,QString mask,QString gateway,QString port,QString loginName);
    bool _IsControllerSameWithOthers(QString ip, int port, QString loginName);
    void _UpdateLC502ListItem(QString name, QString ip, QString mask, QString gateway, QString port, QString loginName);
    void _UpdateLC502InList(QString name, QString ip, QString mask, QString gateway, int port, QString loginName);
    void _DelLC502FromList();
    void _ShowTabWidgetAndLog(bool flag);
    void _UpdateLC502ItemObjName(int index);
    bool _IsExistedController(QString ip,int port);

    void _StartUdpThread();
    void _StopUdpThread();

    void _test_CUdpDataFrame();

    int _dealReadData(CUdpDataFrame data_frame);

    void _WriteData(int command);   //下发指令
    void _ReadData(int command);    //读取反馈

    void _SetControlsEnabled(bool is_enabled,int type = 0);

    void _StartUdpClientThread();
    void _StopUdpClientThread();

signals:
    void ControllerExsitedSig(bool is_existed);
    void CloseUdpThreadSig();
    void WriteDataSig(int index, int commod_code, CLC502Controller *controller); //发送数据给LC502

private:
    Ui::MainWindow *ui;

    bool m_is_init_lc502_list;

    int m_lc502_list_item_index;

    CUdpThread m_udp_thread;//UDP线程，用于收发数据

    CIpSetDialog *m_ip_set_dlg;
    CAddLC502Dialog *m_add_lc502_dlg;
    QList<CLC502Controller *> m_lc502_controllers_list;//key为ip，value为lc502信息
};
#endif // MAINWINDOW_H

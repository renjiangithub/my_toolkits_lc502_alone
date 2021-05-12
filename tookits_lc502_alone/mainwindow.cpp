
#include<QString>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QToolButton>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mystruct.h"
#include "lc502controller.h"
#include "cudpdataframe.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("LC502控制器V1.0.1"));
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);
    _LoadConfig();

    _InitMainWindow();
    _InitDialog();
    //ui->splitter->setStretchFactor(0,97);
    //ui->splitter->setStretchFactor(1,3);
    //ui->splitter->setAutoFillBackground(true);

//     _StartUdpThread();


    _test_CUdpDataFrame();



}

MainWindow::~MainWindow()
{

    delete m_add_lc502_dlg;
    delete m_ip_set_dlg;
    delete ui;
}


void MainWindow::on_add_lc502_btn_clicked()
{
    m_add_lc502_dlg->m_is_add_item = true;
    m_add_lc502_dlg->ShowDlg();
}

void MainWindow::_OnLC502ListChanged(bool is_add_item, QString name, QString ip, QString mask, QString gateway, QString port, QString loginName)
{
    if(is_add_item == true)
    {
        if(_IsExistedController(ip,port.toInt())){
            emit ControllerExsitedSig(true);
            return;
        }
        emit ControllerExsitedSig(false);
        _OnAddLC502Item(name, ip, mask, gateway, port, loginName);
        //ui->lc600_listWidget->setCurrentRow(ui->lc600_listWidget->count() - 1);
        _AddLC502InList(name, ip, mask, gateway, port.toInt(), loginName);
        //TODO: emit AddTcpClientSig(name, ip, mask, gateway, port.toInt(), loginName);
    }
    else
    {
        if(_IsControllerSameWithOthers(ip, port.toInt(), loginName)){
            emit ControllerExsitedSig(true);
            return;
        }//name, ip, mask, gateway, port.toInt(), loginName
        emit ControllerExsitedSig(false);
        _UpdateLC502ListItem(name, ip, mask, gateway, port, loginName);
        _UpdateLC502InList(name, ip, mask, gateway, port.toInt(), loginName);
         //TODO: emit UpdateTcpClientIPSig(m_lc600_list_item_index,m_lc600_controllers_list[m_lc600_list_item_index]);
    }
}

void MainWindow::_InitDialog()
{
    //qDebug()<<"_InitDialog()";
    //初始化添加LC502的弹出窗CAddLC502Dialog
    m_add_lc502_dlg = new CAddLC502Dialog();
    int lc502_list_lsize=m_lc502_controllers_list.size();
    if(lc502_list_lsize>0){
        m_add_lc502_dlg->SetNameIpPortAddr(m_lc502_controllers_list[lc502_list_lsize-1]->m_name,
                QString::number(m_lc502_controllers_list[lc502_list_lsize-1]->m_port),
                m_lc502_controllers_list[lc502_list_lsize-1]->m_ip);
    }
    connect(m_add_lc502_dlg,SIGNAL(AddLC502MsgSig(bool,QString,QString,QString,QString,QString,QString)),this,SLOT(_OnLC502ListChanged(bool,QString,QString,QString,QString,QString,QString)));


    connect(this,SIGNAL(ControllerExsitedSig(bool)),m_add_lc502_dlg,SLOT(OnControllerExsited(bool)));

    //初始化选择本机使用网卡IP的弹出窗
    m_ip_set_dlg = new CIpSetDialog();
}

void MainWindow::_LoadConfig()
{
    QString file_name = "../MyConfigs/LC502Config/lc502_plugin.ini";
    qDebug()<<file_name;
    QFile file(file_name);
    if (!file.exists()) {
        qDebug()<<"缺少配置文件lc502_plugin.ini";
        return;
    }
//    qDebug()<<"找到配置文件lc502_plugin.ini";
    QSettings read_ini(file_name, QSettings::IniFormat);
    read_ini.setIniCodec(QTextCodec::codecForName("UTF_8"));

    g_sudp_property.m_lc502_port = read_ini.value("/Udp/lc502_port",17222).toInt();
    g_sudp_property.m_receive_port = read_ini.value("/Udp/receive_port",17223).toInt();
    g_sudp_property.m_read_timeout = read_ini.value("/Udp/read_timeout",10).toInt();
    g_sudp_property.m_broadcast_use_ip = read_ini.value("/Udp/broadcast_use_ip","127.0.0.1").toString();

    _LoadLC502(read_ini);
    //_LoadLog(read_ini);
}

void MainWindow::_LoadLC502(QSettings &read_ini)
{
//    qDebug()<<"_LoadLC502";
    int num = read_ini.value("/LC502/num").toInt();
//    qDebug()<<num;
    for (int i = 0 ; i < num ; i++) {
        QString itemName = QString("lc502_%1").arg(i);
        QStringList lc502_list= read_ini.value("/LC502/"+itemName).toStringList();

        qDebug()<<lc502_list;
        if(lc502_list.size() == 6) {
            QString name = lc502_list.at(0);
            QString ip = lc502_list.at(1);
            QString mask = lc502_list.at(2);
            QString gateway = lc502_list.at(3);
            int port = lc502_list.at(4).toInt();
            QString loginName = lc502_list.at(5);

//            qDebug()<<" if(lc502_list.size() == 4) {";
            _AddLC502InList(name,ip,mask,gateway,port,loginName);
            _OnAddLC502Item(name,ip,mask,gateway,QString::number(port),loginName);
        }
    }
}

void MainWindow::_AddLC502InList(QString name, QString ip, QString mask, QString gateway, int port, QString loginName)
{
//    qDebug()<<" _AddLC502InList";
    CLC502Controller *ctrl = new CLC502Controller(this);
    ctrl->m_name=name;
    ctrl->m_ip=ip;
    ctrl->m_mask=mask;
    ctrl->m_gateway=gateway;
    ctrl->m_port=port;
    ctrl->m_loginName=loginName;

    m_lc502_controllers_list.append(ctrl);
}

void MainWindow::_OnAddLC502Item(QString name, QString ip, QString mask, QString gateway, QString port, QString loginName)
{
//    qDebug()<<"_OnAddLC502Item";
    QWidget *widget = new QWidget(ui->lc502_listWidget);
    widget->setStyleSheet("QWidget{border:none; border-radius:2px;margin:2px; font-size:11px;font-style: Microsoft YaHei}");
    QGridLayout *layout = new QGridLayout(widget);
    QLabel *name_label = new QLabel(widget);
    QLabel *name_label_msg = new QLabel(widget);
    QLabel *ip_label = new QLabel(widget);
    QLabel *ip_label_msg = new QLabel(widget);
    QLabel *mask_label = new QLabel(widget);
    QLabel *mask_label_msg = new QLabel(widget);
    QLabel *gateway_label = new QLabel(widget);
    QLabel *gateway_label_msg = new QLabel(widget);
    QLabel *port_label = new QLabel(widget);
    QLabel *port_label_msg = new QLabel(widget);
    QLabel *loginName_label = new QLabel(widget);
    QLabel *loginName_label_msg = new QLabel(widget);

    QToolButton *edit_btn = new QToolButton(widget);
    QToolButton *del_btn = new QToolButton(widget);
    QToolButton *authorize_btn = new QToolButton(widget);
    QLabel *comm_status = new QLabel(widget);

    layout->setMargin(2);
    layout->addWidget(name_label,0,0,1,1);
    layout->addWidget(name_label_msg,0,1,1,2);

    layout->addWidget(ip_label,1,0,1,1);
    layout->addWidget(ip_label_msg,1,1,1,2);

    layout->addWidget(mask_label,2,0,1,1);
    layout->addWidget(mask_label_msg,2,1,1,2);

    layout->addWidget(gateway_label,3,0,1,1);
    layout->addWidget(gateway_label_msg,3,1,1,2);

    layout->addWidget(port_label,4,0,1,1);
    layout->addWidget(port_label_msg,4,1,1,2);

    layout->addWidget(loginName_label,5,0,1,1);
    layout->addWidget(loginName_label_msg,5,1,1,2);

    layout->addWidget(comm_status,0,2,1,1);
    layout->addWidget(edit_btn,6,0,1,1);
    layout->addWidget(del_btn,6,1,1,1);
    layout->addWidget(authorize_btn,6,3,1,1);

    int index = ui->lc502_listWidget->count();
    name_label->setObjectName("name_label");   // 设置label名称
    name_label->setText("名称：");
    name_label_msg->setObjectName("name_label" + QString::number(index));   // 设置label名称
    name_label_msg->setText(name);

    ip_label->setObjectName("ip_label");   // 设置label名称
    ip_label->setText("IP：");

    ip_label_msg->setObjectName("ip_label" + QString::number(index));   // 设置label名称
    ip_label_msg->setText(ip);

    mask_label->setObjectName("mask_label");   // 设置label名称
    mask_label->setText("子网掩码：");

    mask_label_msg->setObjectName("mask_label" + QString::number(index));   // 设置mask名称
    mask_label_msg->setText(mask);

    gateway_label->setObjectName("gateway_label");   // 设置label名称
    gateway_label->setText("默认网关：");

    gateway_label_msg->setObjectName("gateway_label" + QString::number(index));   // 设置mask名称
    gateway_label_msg->setText(gateway);

    port_label->setObjectName("port_label");   // 设置label名称
    port_label->setText("端口号：");

    port_label_msg->setObjectName("port_label" + QString::number(index));   // 设置port名称
    port_label_msg->setText(port);

    loginName_label->setObjectName("loginName_label");   // 设置label名称
    loginName_label->setText("登录用户名：");

    loginName_label_msg->setObjectName("loginName_label" + QString::number(index));   // 设置label名称
    loginName_label_msg->setText(loginName);

    //设置通讯状态（授权）指示灯
    comm_status->setObjectName("comm_status" + QString::number(index));
    comm_status->setText("未授权");
    QPixmap pixmap(":/images/comm_abnormal.png");
    pixmap.scaled(QSize(4,4), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    comm_status->setPixmap(pixmap);
    comm_status->setAlignment(Qt::AlignRight);

    //“编辑”按钮
    edit_btn->setObjectName("edit_btn"+ QString::number(index));   // 设置label名称
    edit_btn->setIcon(QIcon(":/images/edit.png"));
    edit_btn->setToolTip(tr("编辑"));
    connect(edit_btn,SIGNAL(clicked()),this,SLOT(_OnEditBtnClicked()));

    //“删除”按钮
    del_btn->setObjectName("del_btn"+ QString::number(index));   // 设置label名称
    connect(del_btn,SIGNAL(clicked()),this,SLOT(_OnDelLc502BtnClicked()));
    del_btn->setIcon(QIcon(":/images/delete.ico"));
    del_btn->setToolTip("删除");

    widget->setLayout(layout);

    QListWidgetItem *aItem = new QListWidgetItem(ui->lc502_listWidget);// 添加自定义的item
    ui->lc502_listWidget->addItem(aItem);
    //aItem->setSizeHint(QSize(75,120));
    ui->lc502_listWidget->setItemWidget(aItem,widget);

}

bool MainWindow::_IsControllerSameWithOthers(QString ip, int port, QString loginName)
{
    for(int i = 0; i < m_lc502_controllers_list.size(); i++)
    {
        if(i != m_lc502_list_item_index){
            if((ip == m_lc502_controllers_list[i]->m_ip
                && port == m_lc502_controllers_list[i]->m_port)
                    || loginName == m_lc502_controllers_list[i]->m_loginName)
            {
                return true;
            }
        }
    }

    return false;
}

void MainWindow::_UpdateLC502ListItem(QString name, QString ip, QString mask, QString gateway, QString port, QString loginName)
{
    QWidget* pwig = ui->lc502_listWidget->itemWidget(ui->lc502_listWidget->item(m_lc502_list_item_index));  // 获取里面的QWidget

    if (pwig != NULL)
    {
        QList<QLabel*> labelList = pwig->findChildren<QLabel*>();  // 获取所有的Qlabel
        foreach(QLabel *label, labelList)
        {
            if(label->objectName() == "name_label" + QString::number(m_lc502_list_item_index))
                label->setText(name);
            else if(label->objectName() == "ip_label" + QString::number(m_lc502_list_item_index))
                label->setText(ip);
            else if(label->objectName() == "mask_label" + QString::number(m_lc502_list_item_index))
                label->setText(mask);
            else if(label->objectName() == "gateway_label" + QString::number(m_lc502_list_item_index))
                label->setText(gateway);
            else if(label->objectName() == "port_label" + QString::number(m_lc502_list_item_index))
                label->setText(port);
            else if(label->objectName() == "loginName_label" + QString::number(m_lc502_list_item_index))
                label->setText(loginName);
        }
    }
}

void MainWindow::_UpdateLC502InList(QString name, QString ip, QString mask, QString gateway, int port, QString loginName)
{
    m_lc502_controllers_list[m_lc502_list_item_index]->m_name = name;
    m_lc502_controllers_list[m_lc502_list_item_index]->m_ip = ip;
    m_lc502_controllers_list[m_lc502_list_item_index]->m_mask = mask;
    m_lc502_controllers_list[m_lc502_list_item_index]->m_gateway = gateway;
    m_lc502_controllers_list[m_lc502_list_item_index]->m_port = port;
    m_lc502_controllers_list[m_lc502_list_item_index]->m_loginName = loginName;
}

void MainWindow::_DelLC502FromList()
{
    m_lc502_controllers_list.erase(m_lc502_controllers_list.begin()+m_lc502_list_item_index);
}

void MainWindow::_ShowTabWidgetAndLog(bool flag)
{
    if(flag == true)
    {
        ui->tabWidget->show();
        ui->log_textBrowser->show();
        ui->clear_log_btn->show();
        ui->hide_log_btn->show();
    }
    else
    {
        ui->tabWidget->hide();
        ui->log_textBrowser->hide();
        ui->clear_log_btn->hide();
        ui->hide_log_btn->hide();
    }
}

void MainWindow::_UpdateLC502ItemObjName(int index)
{
    for (int i = index; i < ui->lc502_listWidget->count(); i++) {
        QWidget* pwig = ui->lc502_listWidget->itemWidget(ui->lc502_listWidget->item(i));  // 获取里面的QWidget

        if (pwig != NULL)
        {
            QList<QLabel*> labelList = pwig->findChildren<QLabel*>();  // 获取所有的Qlabel
            foreach(QLabel *label, labelList)
            {
                if(label->objectName() == "name_label" + QString::number(i+1))
                    label->setObjectName("name_label" + QString::number(i));
                else if(label->objectName() == "ip_label" + QString::number(i+1))
                   label->setObjectName("ip_label" + QString::number(i));
                else if(label->objectName()=="mask_label" + QString::number(i+1))
                  label->setObjectName("mask_label" + QString::number(i));
                else if(label->objectName()=="gateway_label" + QString::number(i+1))
                    label->setObjectName("gateway_label" + QString::number(i));
                else if(label->objectName() == "port_label" + QString::number(i+1))
                    label->setObjectName("port_label" + QString::number(i));
                else if(label->objectName() == "loginName_label" + QString::number(i+1))
                    label->setObjectName("loginName_label" + QString::number(i));
                else if(label->objectName() == "comm_status" + QString::number(i+1))
                    label->setObjectName("comm_status" + QString::number(i));
            }
            QList<QToolButton*> btnList = pwig->findChildren<QToolButton*>();  // 获取所有的Qlabel
            foreach(QToolButton *button, btnList)
            {
                if(button->objectName() == "edit_btn" + QString::number(i+1))
                    button->setObjectName("edit_btn" + QString::number(i));
                else if(button->objectName() == "del_btn" + QString::number(i+1))
                    button->setObjectName("del_btn" + QString::number(i));
            }
        }
    }
}

bool MainWindow::_IsExistedController(QString ip, int port)
{
    for(int i = 0; i < m_lc502_controllers_list.size(); i++)
    {
        if((ip == m_lc502_controllers_list[i]->m_ip
                && port == m_lc502_controllers_list[i]->m_port))
        {
            return true;
        }
    }

    return false;
}

void MainWindow::_StartUdpThread()
{
    connect(&m_udp_thread,SIGNAL(SendLC502UdpReadDataSig(QVector<CUdpDataFrame>)),this,SLOT(_OnSendLC502UdpReadData(QVector<CUdpDataFrame>)));
    connect(this,SIGNAL(CloseUdpThreadSig()),&m_udp_thread,SLOT(OnCloseUdpThread()),Qt::QueuedConnection);
//    connect(&m_udp_thread,SIGNAL(SendLogMsgSig(int,QString)),this,SLOT(_OnShowLogMsg(int,QString)));
//    connect(&m_udp_thread,SIGNAL(SendUdpSearchErrorSig()),this,SLOT(_OnSendUdpSearchError()));

    m_udp_thread.start();
    //ui->lc502_list_label->setText("正在搜索LC600...");



}

void MainWindow::_StopUdpThread()
{

    emit CloseUdpThreadSig();
    m_udp_thread.Quit();
    m_udp_thread.wait();
}

void MainWindow::_test_CUdpDataFrame()
{
    CUdpDataFrame udpframe;
    QByteArray sendPlainData = QByteArray("ABC");
    QByteArray sendcryData = udpframe.sendUdpDatagram(sendPlainData);

    //暂假通过网络收到了数据帧
    QByteArray recvCryData(sendcryData);
    _dealReadData(recvCryData);
}

int MainWindow::_dealReadData(CUdpDataFrame data_frame)
{

}

void MainWindow::_OnDelLc502BtnClicked()
{
    QObject* o = sender();
    QString strName = o->objectName();
    m_lc502_list_item_index = strName.remove("del_btn").toInt();
    QListWidgetItem *pItem = ui->lc502_listWidget->item(m_lc502_list_item_index);//注意：上面删除了一个Item后，删除的Item后面所有Item的index都会发生变化。
    ui->lc502_listWidget->removeItemWidget(pItem);
    delete pItem;

    _DelLC502FromList();

    _UpdateLC502ItemObjName(m_lc502_list_item_index);

    if(m_lc502_controllers_list.isEmpty())
       _ShowTabWidgetAndLog(false);

}

void MainWindow::_OnEditBtnClicked()
{
    QObject* o = sender();//通过 sender()得到当前是谁invoke了你的slot，对应的是QObject->d->sender.
    QString strName = o->objectName();
    //qDebug()<<strName;
    m_lc502_list_item_index = strName.remove("edit_btn").toInt();
    QWidget* pwig = ui->lc502_listWidget->itemWidget(ui->lc502_listWidget->item(m_lc502_list_item_index));  // 获取里面的QWidget
    QString name,ip,mask,gateway,port,loginName;
    if (pwig != NULL)
    {
        QList<QLabel*> labelList = pwig->findChildren<QLabel*>();  // 获取所有的Qlabel
        foreach(QLabel *label, labelList)
        {
            if(label->objectName() == "name_label" + QString::number(m_lc502_list_item_index))
                name = label->text();   // 获取表中的值
            else if(label->objectName() == "ip_label" + QString::number(m_lc502_list_item_index))
                ip = label->text();
            else if(label->objectName()=="mask_label" + QString::number(m_lc502_list_item_index))
                mask = label->text();
            else if(label->objectName()=="gateway_label" + QString::number(m_lc502_list_item_index))
                gateway = label->text();
            else if(label->objectName() == "port_label" + QString::number(m_lc502_list_item_index))
                port = label->text();
            else if(label->objectName() == "loginName_label" + QString::number(m_lc502_list_item_index))
                loginName = label->text();
        }
    }
    m_add_lc502_dlg->m_is_add_item = false;
    m_add_lc502_dlg->SetEditMsg(name,ip,mask,gateway,port,loginName);
}

void MainWindow::_InitMainWindow()
{
    m_is_init_lc502_list = false;;
    ui->toolBar->setStyleSheet("QToolBar {background: #4e607e;spacing: 3px; }");


    ui->clear_log_btn->setToolTip("清除日志");
    ui->hide_log_btn->setToolTip("隐藏日志");


}

void MainWindow::on_ip_set_action_triggered()
{
    m_ip_set_dlg->ShowDlg();
    //qDebug()<<"on_ip_set_action_triggered";
}

void MainWindow::_OnSendLC502UdpReadData(QVector<CUdpDataFrame> udp_data_frame)
{
    QString recResult;
    for(int i = 0; i < udp_data_frame.size(); i++)
    {
//        recResult = QString("%1").arg(udp_data_frame[i].m_headFlag);
    }
    qDebug()<<"recResult   "<<recResult;
    //ui->label_test_udp_rec->setText(recResult);
    _StopUdpThread();
}

#ifndef CUDPTHREAD_H
#define CUDPTHREAD_H

#include "cudpdataframe.h"
#include "lc502controller.h"
#include "Udpclient.h"


#include <QMutex>
#include <QObject>
#include <QThread>
#include <QUdpSocket>




class CUdpThread : public QThread
{
    Q_OBJECT
public:
    explicit CUdpThread(QObject *parent = nullptr);

    void run() override;

    int Write(); //发送命令#1
    void Read(); //接收回复数据

    void AnalyReadData();

    void Quit();

signals:
    void SendLC502UdpReadDataSig(QVector<CUdpDataFrame> udp_data_frame);//将收集到的数据整理传给主线程


public slots:
    void OnCloseUdpThread();
    void OnWriteData(int index, int commod_code, CLC502Controller *controller);//发送数据给LC502

public:
    QSharedPointer<QUdpSocket> m_socket;
    QByteArray m_data; //存储收到的数据
    bool m_is_quit; //停止标志位
    int m_read_times; //读取超时时间
    QMutex m_mutex;

//    QList<QSharedPointer<Udpclient>> m_lc502_udp_client_list; //每个LC502对应一个Udpclient
    Udpclient udpC;
};

#endif // CUDPTHREAD_H

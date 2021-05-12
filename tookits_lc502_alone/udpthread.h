#ifndef CUDPTHREAD_H
#define CUDPTHREAD_H

#include "cudpdataframe.h"

#include <QMutex>
#include <QObject>
#include <QThread>
#include <QUdpSocket>

class CUdpThread : public QThread
{
    Q_OBJECT
public:
    explicit CUdpThread(QObject *parent = nullptr);
    ~CUdpThread();

    void run() override;

    int Write(); //发送命令#1
    void Read(); //接收回复数据

    void AnalyReadData();

    void Quit();

signals:
    void SendLC502UdpReadDataSig(QVector<CUdpDataFrame> udp_data_frame);//将收集到的数据整理传给主线程


public slots:
    void OnCloseUdpThread();


public:
    QSharedPointer<QUdpSocket> m_socket;
    QByteArray m_data; //存储收到的数据
    bool m_is_quit; //停止标志位
    int m_read_times; //读取超时时间
    QMutex m_mutex;
};

#endif // CUDPTHREAD_H

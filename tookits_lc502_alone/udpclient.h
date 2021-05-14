#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <cudpdataframe.h>
#include <deque>
#include <qudpsocket.h>

enum UdpAuthorizeStatus
{
    UNAUTHORIZED = 0, //未授权
    AUTHORIZEDING = 1, //正在申请授权
    AUTHORIZED = 2 //已授权
};


class Udpclient : public QObject
{
    Q_OBJECT
public:
    explicit Udpclient(QObject *parent = nullptr);

    void ExecTask();    //执行发送任务
    void _WaitForExecTask();    //有正在执行的任务，等待

    void Write(QByteArray m_final_send_frame, int m_final_send_frame_length);//最终发送
    QByteArray Read(); //接收回复数据

public:

    int m_authorize_status; //0未授权,1正在申请授权，2已授权

    bool m_has_execting_task;   //当前是否有任务在执行

    //    std::deque<CUdpDataFrame> m_write_queue;//保存等待发送的命令帧对象, 本项目采用阻塞方式，不使用任务队列

    CUdpDataFrame m_write_frame;    //等待发送的命令帧对象,注意，这里只有一个，非队列


    QSharedPointer<QUdpSocket> m_socket;   //增加



signals:

public slots:
};

#endif // UDPCLIENT_H

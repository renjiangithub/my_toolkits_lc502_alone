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
    void SendLC502UdpReadDataSig(QByteArray recv_command_code, QByteArray recv_answer_code, QByteArray recv_data_params);//将收集到的数据整理传给主线程

public slots:
    void OnCloseUdpThread();
    void OnWriteData(int index, int commod_code, CLC502Controller *controller);//发送数据给LC502

public:
    QSharedPointer<QUdpSocket> m_socket;
    QByteArray m_data; //存储收到的数据
    bool m_is_quit; //停止标志位
    int m_read_times; //读取超时时间
    QMutex m_mutex;

    QByteArray m_recv_iv; //获取"IV"(密钥)

//    QList<QSharedPointer<Udpclient>> m_lc502_udp_client_list; //每个LC502对应一个Udpclient
    Udpclient udpC;

    CUdpDataFrame data_frame;

private:
//    QByteArray _uint16ToQByteArray(uint16_t intVar);

    QByteArray _get_cryptographic_type(QByteArray data); //获取"加密模式"
    QByteArray _get_plain_effective_length(QByteArray data); //获取"有效明文长度"
    QByteArray _get_iv(QByteArray data);//获取"IV"(密钥)
    QByteArray _get_cryptographic_data(QByteArray data);//获取"保密信息密文"
    QByteArray _get_plain_data(QByteArray data, QByteArray iv);//获取解密后的"有效明文+填充数据"
    QByteArray _get_effective_data(QByteArray data, QByteArray length); //获取解密后的"有效明文"
    QByteArray _get_command_code(QByteArray data); //获取"命令码"
    QByteArray _get_answer_code(QByteArray data); //获取"应答码"
    QByteArray _get_data_params(QByteArray data); //获取"数据参数"

    QByteArray _getDecryptCode(QByteArray cryptData, QByteArray iv);   //对“有效明文+填充数据”进行解密得明文
};

#endif // CUDPTHREAD_H

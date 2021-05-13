#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QMutex>
#include <QString>
#include <QTcpSocket>
#include <QUdpSocket>



class Communication
{
public:
    Communication();
    ~Communication();

    bool DeviceReadFixedCount_Com(QByteArray *pBuffer, int& dwCount, int FixedCount);//读取固定字节数 modify 20160520
    bool DeviceRead(QByteArray *pBuffer, int *dwCount, int MaxCount);
    bool DeviceWrite(QByteArray* data,int length);
    void DeviceClose();
    bool DeviceOpen();
    bool SendCommand(QByteArray* tempBuf,int tempLength,QByteArray* revBuf,QByteArray& revLength,
                     QByteArray maxCount,QByteArray& reValue,QByteArray commandType);//向服务器发送一条命令的过程。成功返回TRUE

private:
    bool _DeviceRead_Com(QByteArray *pBuffer, QByteArray *dwCount, QByteArray MaxCount);
    bool _DeviceRead_Tcp(QByteArray *pBuffer, QByteArray *dwCount, QByteArray MaxCount);
    bool _DeviceRead_Udp(QByteArray *pBuffer, QByteArray *dwCount, QByteArray MaxCount);

    bool _DeviceWrite_Com(QByteArray* data,int length);
    bool _DeviceWrite_Tcp(QByteArray* data,int length);
    bool _DeviceWrite_Udp(QByteArray* data,int length);

    void _DeviceClose_Com();
    void _DeviceClose_Tcp();
    void _DeviceClose_Udp();

    bool _DeviceOpen_Com();
    bool _DeviceOpen_Tcp();
    bool _DeviceOpen_Udp();

public:
    int     m_CommuType;//0--串口，1--TCP, 2--UDP
    int     m_Timeout;  //超时毫秒
    int     m_ComPort;  //串口号
    int     m_Baudrate; //串口波特率
    QString m_IPAddr;   //网络IP
    int     m_IPPort;   //网络端口号

    bool     m_QuitFlag;//TRUE表示要退出

public:
    QSharedPointer<QTcpSocket> m_TcpSocket;    //Tcp通信用sokcet
    QSharedPointer<QUdpSocket> m_UdpSocket;    //Udp通信用sokcet
    QMutex m_mutex;

};

#endif // COMMUNICATION_H

#include "cudpdataframe.h"
#include "udpthread.h"
#include "mystruct.h"

//int udp_frame = qRegisterMetaType<QVector<CUdpDataFrame>>("QVector<CUdpDataFrame>");


CUdpThread::CUdpThread(QObject *parent) : QThread(parent)
{
    m_is_quit = false;
    m_read_times = 0;

}

CUdpThread::~CUdpThread()
{

}
void CUdpThread::run()
{
    m_socket = QSharedPointer<QUdpSocket>(new QUdpSocket());
    m_is_quit = false;
    m_read_times = 0;
    m_data.clear();

    if(!m_socket->bind(QHostAddress(g_sudp_property.m_broadcast_use_ip),g_sudp_property.m_receive_port))
    {
        qDebug()<<"UDP bind出错！";
        //TODO:emit SendLogMsgSig(2,"udp bind "+g_sudp_property.m_broadcast_use_ip + ":" + QString::number(g_sudp_property.m_receive_port) + " failed !");
        //TODO:emit SendUdpSearchErrorSig();
        return;
    }
     qDebug()<<"UDP bind 成功！";
    if(Write() != -1)
        Read();
    AnalyReadData();
}

int CUdpThread::Write()
{
    //关于UDP单播，组播，广播：https://blog.csdn.net/pingis58/article/details/82977335
    int ret = m_socket->writeDatagram("#1",QHostAddress("192.168.158.17"),g_sudp_property.m_lc502_port);//这里本机测试用QHostAddress::LocalHost
//TODO:    emit SendLogMsgSig(2,"write data : #1");
//     qDebug()<<"QHostAddress::LocalHost："<<QHostAddress("192.168.158.17")<<"   g_sudp_property.m_lc502_port:  "<<g_sudp_property.m_lc502_port;
    qDebug()<<"Write() ret: "<<ret;
    return ret;
}

void CUdpThread::Read()
{
    while (1) {
        if(m_is_quit == true || m_read_times >= g_sudp_property.m_read_timeout){
            qDebug()<<"CUdpThread::Read()  break";
            break;
        }
        if(m_socket->waitForReadyRead(5000)){
            QByteArray datagram;
            datagram.resize(m_socket->pendingDatagramSize());
            m_socket->readDatagram(datagram.data(),datagram.size());
            //TODO: emit SendLogMsgSig(2,"receive data : " + datagram.toHex());
            m_data.push_back(datagram);
            qDebug()<<"CUdpThread::Read() "<<datagram;

        }
        m_read_times++;
    }
}

void CUdpThread::AnalyReadData()
{
    QString hex_str = m_data.toHex();
    QStringList str_list = hex_str.split(",");

    QVector<CUdpDataFrame> udp_data_frame;
    for(int i = 0; i < str_list.size(); i++)
    {
        QString frame = str_list[i];
        CUdpDataFrame data_frame;
        if(!data_frame.IsValidFrame(frame))
            continue;

        udp_data_frame.push_back(data_frame);
    }

    emit SendLC502UdpReadDataSig(udp_data_frame);
}

void CUdpThread::Quit()
{
    m_mutex.lock();
    m_is_quit = true;
    m_mutex.unlock();
}

void CUdpThread::OnCloseUdpThread()
{
    m_data.clear();
    m_socket->close();
}

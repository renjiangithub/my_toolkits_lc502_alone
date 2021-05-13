#include "cudpdataframe.h"
#include "udpthread.h"
#include "mystruct.h"
#include "command.h"
//int udp_frame = qRegisterMetaType<QVector<CUdpDataFrame>>("QVector<CUdpDataFrame>");

using namespace sansi::protocol;

CUdpThread::CUdpThread(QObject *parent) : QThread(parent)
{
    m_is_quit = false;
    m_read_times = 0;

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

    //获取
    if(Write() != -1)
        Read();
    AnalyReadData();
}

int CUdpThread::Write()
{
    //关于UDP单播，组播，广播：https://blog.csdn.net/pingis58/article/details/82977335
    int ret = m_socket->writeDatagram("hello world",QHostAddress("192.168.158.17"),g_sudp_property.m_lc502_port);//这里本机测试用QHostAddress::LocalHost
    //TODO:    emit SendLogMsgSig(2,"write data : #1");
    qDebug()<<"QHostAddress::LocalHost："<<QHostAddress("192.168.158.17")<<"   g_sudp_property.m_lc502_port:  "<<g_sudp_property.m_lc502_port;
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

//发送数据给LC502
void CUdpThread::OnWriteData(int index, int commod_code, CLC502Controller *controller)
{
    qDebug()<<"CUdpThread::OnWriteData(int index, int commod_code, CLC502Controller *controller)";
    CUdpDataFrame data_frame;
    data_frame.SetClientCommand(commod_code);
    QByteArray send_crypt_frame;    //加密后的完整发送帧
    //注意：不同于TCP需要时刻保持连接，必须验证，只有在CONNECTED状态下才允许发送，
    //这里使用UDP，不存在连接问题，只有授权状态，未授权时，通过禁用当前LC502右侧面板方式，防止发出读写指令
     bool is_right_command = true;

    switch (commod_code)
    {
    case Command::GET_SERIAL_PORT_PARAMS:{
        QByteArray effective_plain;
        effective_plain.append(0x04);
        effective_plain.append(0xFF);
        effective_plain.append(0x1008);
        effective_plain.append(0x1009);
        effective_plain.append(0x100a);
        send_crypt_frame = data_frame.getSendUdpDatagram(effective_plain);  //返回完整的发送帧
        qDebug()<<"*********send_crypt_frame:"<<send_crypt_frame;
    }
        break;
    case Command::SET_SERIAL_PORT_PARAMS:
    case Command::GET_AR502_SIDE_PARAMS:
    case Command::SET_AR502_SIDE_PARAMS:
    case Command::GET_X9_SIDE_PARAMS:
    case Command::SET_X9_SIDE_PARAMS:
    case Command::GET_COMMON_REGISTER_DATA:
    case Command::ASYNC_LC502_TIME:
    case Command::RESET_LC502_TIME:
    case Command::RESTORE_LC502_FACTORY_SETTING:
    case Command::CLEAR_LC502_DATA:
    case Command::GET_CHANNEL_DATA:
    case Command::SET_CHANNEL_1_LIGHTING:
    case Command::GET_CHANNEL_1_FEEDBACK:
    case Command::SET_CHANNEL_2_LIGHTING:
    case Command::GET_CHANNEL_2_FEEDBACK:
    case Command::GET_LC502_REGISTER:
    case Command::SET_DO_1_SWITCH:
    case Command::GET_DO_1_FEEDBACK:
    case Command::SET_DO_2_SWITCH:
    case Command::GET_DO_2_FEEDBACK:
    default:
        qDebug()<<"command code is not exist.";
        is_right_command = false;
        break;
    }
    data_frame.m_final_send_frame=send_crypt_frame;
    data_frame.m_final_send_frame_length=send_crypt_frame.size();
    qDebug()<<"**************************************:";
    if(is_right_command)  {
//        m_lc502_udp_client_list[index]->ExecTask();
        udpC.m_write_frame=data_frame;
        udpC.ExecTask();
    }
}

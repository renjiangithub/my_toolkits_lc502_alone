#include "cudpdataframe.h"
#include "udpthread.h"
#include "mystruct.h"
#include "command.h"
#include "crc16_sansi.h"
#include "caesclass.h"
#include "Common.h"

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
    //获取
    if(Write() != -1){
        Read();
    }
    AnalyReadData();
}

int CUdpThread::Write()
{
    //    qDebug()<<"m_final_send_frame：" << this->data_frame.m_final_send_frame;
    int ret = m_socket->writeDatagram(this->data_frame.m_final_send_frame,QHostAddress("192.168.158.17"),g_sudp_property.m_lc502_port);//这里本机测试用QHostAddress::LocalHost
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
            //            qDebug()<<"datagram.size():"<<datagram.size();
            m_socket->readDatagram(datagram.data(),datagram.size());
            //TODO: emit SendLogMsgSig(2,"receive data : " + datagram.toHex());
            m_data.push_back(datagram);
            qDebug()<<"CUdpThread::Read() "<<datagram;
            break;
        }
        m_read_times++;
    }
}

void CUdpThread::AnalyReadData()
{
    qDebug()<<"CUdpThread::AnalyReadData():";
    QString hex_str = m_data.toHex();

    //检查长度，收到的数据包大小至少为"帧起始符" "有效长度" "加密参数"  "校验码" "结束符"长度和，不含"保密信息密文"，累计21Byte
    if(m_data.size()<=21){
        qDebug()<<"收到数据帧长度不够";
        return;
    }
    //检查"帧起始符"和"结束符"
    if(QString(m_data.mid(0,1).toHex().toUpper())!="A5" || QString(m_data.mid(m_data.size()-1,1).toHex().toUpper())!="5A"){
        qDebug()<<"收到数据帧起始符或结束符不对";
        return;
    }
    //检查校验，"帧起始符"到"保密信息密文"的crc校验结果与校验码对比
    int wait_check_data_length = m_data.size()-3;
    QByteArray wait_check_data_received = m_data.mid(0,wait_check_data_length);//接收到的待校验内容
    QByteArray check_code_received =  m_data.mid(m_data.size()-3,2);//接收到的数据帧中的校验码
    uint16_t crc = gen_crc16_redis((const uint8_t *)wait_check_data_received.constData(), wait_check_data_received.length()); //求出来的校验码
    QByteArray check_code_calculated = QByteArray(_uint16ToQByteArray(crc));
    if(check_code_received!=check_code_calculated){
        qDebug()<<"收到数据帧校验不通过";
        return;
    }
    //获取"加密模式"
    QByteArray recv_cryptographic_type = _get_cryptographic_type(m_data);

    //获取"有效明文长度"
    QByteArray recv_plain_effective_length = _get_plain_effective_length(m_data);

    //获取"IV"(密钥)
    QByteArray recv_iv = _get_iv(m_data);

    //获取"保密信息密文"
    QByteArray recv_cryptographic_data = _get_cryptographic_data(m_data);

    //获取解密后的"有效明文+填充数据"
    QByteArray recv_plain_data = _get_plain_data(recv_cryptographic_data,this->m_recv_iv);

    //获取解密后的"有效明文"
    QByteArray recv_effective_data = _get_effective_data(recv_plain_data,recv_plain_effective_length);

    //获取"命令码"
    QByteArray recv_command_code = _get_command_code(recv_effective_data);

    //获取"应答码"
    QByteArray recv_answer_code = _get_answer_code(recv_effective_data);

    //获取"数据参数"
    QByteArray recv_data_params = _get_data_params(recv_effective_data);


    emit SendLC502UdpReadDataSig(recv_command_code, recv_answer_code, recv_data_params);
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

    this->data_frame.SetClientCommand(commod_code);
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
        effective_plain.append(_uint16ToQByteArray(0x1008));
        effective_plain.append(_uint16ToQByteArray(0x1009));
        effective_plain.append(_uint16ToQByteArray(0x100a));
        send_crypt_frame = data_frame.getSendUdpDatagram(effective_plain);  //返回完整的发送帧
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
    this->data_frame.m_final_send_frame=send_crypt_frame;
    this->data_frame.m_final_send_frame_length=send_crypt_frame.size();
    if(is_right_command)  {
        //        m_lc502_udp_client_list[index]->ExecTask();
        //        udpC.m_write_frame=data_frame;
        //        udpC.ExecTask();



    }
}

//获取"加密模式"
QByteArray CUdpThread::_get_cryptographic_type(QByteArray data)
{
    QByteArray type = data.mid(3,1);
    if(QString(type.toHex().toUpper())!="00" && QString(type.toHex().toUpper())!="01"){
        qDebug()<<"收到数据帧\"加密模式\"不正确";
        return NULL;
    }
    qDebug()<<"收到数据帧\"加密模式\"为"<<type;
    return type;
}

//获取"有效明文长度"
QByteArray CUdpThread::_get_plain_effective_length(QByteArray data)
{
    QByteArray length = data.mid(4,2);
    qDebug()<<"收到数据帧\"有效明文长度\"为"<<length;
    return length;
}

//获取"IV"(密钥)
QByteArray CUdpThread::_get_iv(QByteArray data)
{
    QByteArray iv = data.mid(6,16);
    qDebug()<<"收到数据帧\"IV(密钥)\"为"<<iv;
    this->m_recv_iv = iv;
    return iv;
}

//获取"保密信息密文"
QByteArray CUdpThread::_get_cryptographic_data(QByteArray data)
{
    int cryptographic_data_length=data.size()-22-3;
    QByteArray cryptographic_data = data.mid(22,cryptographic_data_length);
    qDebug()<<"收到\"保密信息密文\"为"<<cryptographic_data;

    return cryptographic_data;
}
//获取解密后的"有效明文+填充数据"
QByteArray CUdpThread::_get_plain_data(QByteArray recv_cryptographic_data, QByteArray iv)
{
    //recv_cryptographic_data为加密过的"保密信息密文"，解密得明文

    QByteArray plain = _getDecryptCode(recv_cryptographic_data, iv);
    return plain;
}

//获取解密后的"有效明文"
QByteArray CUdpThread::_get_effective_data(QByteArray plain_data, QByteArray length)
{
    int len = length.toHex().toInt();
    QByteArray effective_data = plain_data.mid(0,len);
    return effective_data;
}


//获取"命令码"
QByteArray CUdpThread::_get_command_code(QByteArray plain_data)
{
    QByteArray command_code = plain_data.mid(0,1);
    qDebug()<<"命令码: "<<command_code;
    return command_code;
}

//获取"应答码"
QByteArray CUdpThread::_get_answer_code(QByteArray plain_data)
{
    QByteArray answer_code = plain_data.mid(1,1);
    qDebug()<<"应答码: "<<answer_code;
    return answer_code;
}

//获取"数据参数"
QByteArray CUdpThread::_get_data_params(QByteArray plain_data)
{
    QByteArray data_params = plain_data.mid(2,plain_data.size()-2);
    qDebug()<<"数据参数: "<<data_params;
    return data_params;
}
//对“有效明文+填充数据”进行解密得明文
QByteArray CUdpThread::_getDecryptCode(QByteArray cryptData, QByteArray iv)
{
    //把QByteArray形式得密钥iv转为unsigned char*形式得key
    unsigned char* key = (unsigned char*)malloc(16*sizeof(char));
    memset(key,'0',16);
    //转存
    for(int i=0; i<16; i++)
        key[i]=iv.at(i);

    //把QByteArray形式得密文cryptData转为unsigned char*形式得crypt
    unsigned char* crypt = (unsigned char*)malloc(16*sizeof(char));
    memset(crypt,'0',16);
    //转存
    for(int i=0; i<16; i++)
        crypt[i]=cryptData.at(i);

    _print4x4Data(QString("解密用的密钥："),key);
    _print4x4Data(QString("解密前的密文："),crypt);
    Aes aes256(16,key);

    // 解密，输出解密后的明文
    unsigned char* output = (unsigned char*)malloc(16*sizeof(char));
    memset(output,0,16);
    aes256.InvCipher(crypt, output);    //解密
    _print4x4Data(QString("解密后的明文："),output);

    //将结果转为QByteArray
    QByteArray res;
    res.resize(16);
    for(int i=0; i<16; ++i)
        res[i]=output[i];
//    qDebug()<<"_getDecryptCode返回的解密后的QByteArray:"<<res;

    return  res;
}

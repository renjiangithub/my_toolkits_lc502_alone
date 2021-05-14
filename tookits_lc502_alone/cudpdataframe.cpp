#include "cudpdataframe.h"
#include "crc16_sansi.h"
#include <QDebug>
#include "caesclass.h"
#include <iostream>

unsigned char k[] = {0x2b, 0x7e, 0x15, 0x16,
                      0x28, 0xae, 0xd2, 0xa6,
                      0xab, 0xf7, 0x15, 0x88,
                      0x09, 0xcf, 0x4f, 0x3c};

CUdpDataFrame::CUdpDataFrame()
{
    this->m_headFlag.resize(1);
    this->m_headFlag[0]=0xA5;

    this->m_tailFlag.resize(1);
    this->m_tailFlag[0]=0x5A;

    this->m_cryptographic_type.resize(1);
    this->m_cryptographic_type[0]=0x00;

    key= k;
    for(int i=0; i<16; ++i){
        m_IV.append(_uint8ToQByteArray(key[i]));
    }

}

void CUdpDataFrame::testInitial(){
    //    this->m_effective_length=0;//TODO
    //    this->m_m_cryptographic_type=0x00;  //固定加密模式
    //    int m_effective_length_int=5;   //暂定有效铭文长度，为5 char
    //    this->m_m_effective_plain_length=m_effective_length_int;
    //    this->m_m_aes_iv="01234567890123456789012345678901";//iv长度为16 Byte
    //    this->m_cryptographic_data=hexs
    ;//0x12345;
}

void CUdpDataFrame::SetClientCommand(int command)
{
    m_client_command = command;
}

//对有效明文进行填充，使其长度为16的整倍数
QByteArray CUdpDataFrame::_fill(QByteArray plain_effective_data)
{
    QByteArray tmp;
    if(plain_effective_data.size()%16!=0){
        int gap = (plain_effective_data.size()/16+1)*16-plain_effective_data.size();    //后面要填充多少个Byte
        tmp.append(plain_effective_data);
        tmp.append(QByteArray(gap,'0'));
    }

    return tmp;
}
bool CUdpDataFrame::IsValidFrame(QString data)
{
    return true;
}

QByteArray CUdpDataFrame::getSendUdpDatagram(QByteArray plain_data)
{
    //第1步：拼接"保密信息密文"
    //TODO:经AES256加密后的“保密信息密文”
    qDebug()<<"plain_data: "<<plain_data;
    this->m_plain_after_fill = _fill(plain_data); //有效明文+填充数据
    qDebug()<<"m_plain_after_fill: "<<this->m_plain_after_fill;


//    qDebug()<<"m_plain_after_fill.length()："<<m_plain_after_fill.length();
    QByteArray crypt = _getEncryptCode(m_plain_after_fill);
//    QByteArray plain = _getDecryptCode(crypt);

    this->m_cryptographic_data.append(crypt);//加密了
        qDebug()<<"保密信息密文: m_cryptographic_data  "<<m_cryptographic_data;

    //第2步：拼接“加密参数”
    QByteArray cryptographic_params;
    cryptographic_params.append(m_cryptographic_type);  //加密模式0x00;
    //    qDebug()<<"加密模式：m_cryptographic_type: "<<m_cryptographic_type;

    //有效明文长度
    uint16_t plain_data_size=plain_data.size();
    this->m_plain_effective_length = _uint16ToQByteArray(plain_data_size); //"有效明文"长度
    //    qDebug()<<"m_plain_effective_length: "<<this->m_plain_effective_length;
    cryptographic_params.append( this->m_plain_effective_length);   //"有效明文"长度

    cryptographic_params.append(this->m_IV);    //IV

    this->m_cryptographic_params = cryptographic_params;
        qDebug()<<"加密参数: m_cryptographic_params  "<<m_cryptographic_params;

    //第3步：拼接“有效长度”
    int effective_length = m_cryptographic_params.size()+m_cryptographic_data.size();
    this->m_effective_length = _uint16ToQByteArray(effective_length);
    //    qDebug()<<"有效长度:m_effective_length: "<<m_effective_length.toHex();

    //第4步：拼接“校验码”
    m_check_code_crc = _getCrcResult();
    //    qDebug()<<"校验码:m_check_code_crc: "<<m_check_code_crc;


    //第5步：拼接完整发送数据帧
    QByteArray sendUdpData;
    sendUdpData = _sendUdpDataJoint();


    _sendUdpDataSeparator(sendUdpData);  //用分隔符分离开，方便查看

//    qDebug()<<"完整发送数据帧："<<sendUdpData;
//    qDebug()<<"完整发送数据帧："<<sendUdpData.toHex();

    this->m_final_send_frame = sendUdpData;
    this->m_final_send_frame_length = sendUdpData.size();

    return sendUdpData;
}

QByteArray  CUdpDataFrame::_intToByte(int number)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar)  (0x000000ff & number);
    abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
    return abyte0;
}

//计算发送帧的校验码
QByteArray CUdpDataFrame::_getCrcResult()
{
    QByteArray datagram;
    datagram.append(this->m_headFlag);  //帧起始符
    datagram.append(this->m_effective_length);  //有效长度
    datagram.append(this->m_cryptographic_params);  //加密参数
    datagram.append(this->m_cryptographic_data);  //保密信息密文

//    qDebug()<<"发送前的待校验内容："<<datagram;
    uint16_t crc = gen_crc16_redis((const uint8_t *)datagram.constData(), datagram.length());
//    qDebug()<<"发送前的校验码："<<_uint16ToQByteArray(crc);
    return QByteArray(_uint16ToQByteArray(crc));
}
//将uint8_t转为Qbytearray
QByteArray CUdpDataFrame::_uint8ToQByteArray(uint8_t intVar)
{
    QByteArray array;
    int len_intVar = sizeof(intVar);
    array.resize(len_intVar);
    memcpy(array.data(), &intVar, len_intVar);
    QByteArray result;
    result.append(array.mid(0,1));
    //    qDebug()<<result;
    return result;
}
//将uint16_t转为Qbytearray
QByteArray CUdpDataFrame::_uint16ToQByteArray(uint16_t intVar)
{
    QByteArray array;
    int len_intVar = sizeof(intVar);
    array.resize(len_intVar);
    memcpy(array.data(), &intVar, len_intVar);
    QByteArray result;
    result.append(array.mid(1,1)).append(array.mid(0,1));
    //    qDebug()<<result;
    return result;
}
//将拼接好的发送帧用分隔符分离开，方便查看
QByteArray CUdpDataFrame::_sendUdpDataSeparator(QByteArray Data)
{
    QByteArray byteArray(Data);
    byteArray.insert(1,' ');
    byteArray.insert(4,' ');
    byteArray.insert(6,' ');
    byteArray.insert(9,' ');
    byteArray.insert(26,' ');
    byteArray.insert(byteArray.size()-1,' ');
    byteArray.insert(byteArray.size()-4,' ');
    qDebug()<<"添加分割符的发送数据帧："<<byteArray;
//        qDebug()<<"添加分割符的发送数据帧："<<byteArray.toHex();
    return byteArray;
}

//对“有效明文+填充数据”进行明文加密得密文
QByteArray CUdpDataFrame::_getEncryptCode(QByteArray plainData)
{
    //    qDebug()<<"传入_getEncryptCode的plainData:"<<plainData;
    //    qDebug()<<"传入_getEncryptCode的plainDat.toHex():"<<plainData.toHex();

    unsigned char* plain = (unsigned char*)malloc(16*sizeof(char));
    memset(plain,'0',16);

    //转存
    for(int i=0; i<16; i++)
        plain[i]=plainData.at(i);


    _print4x4Data(QString("发送前的密钥："),key);
    _print4x4Data(QString("发送前加密前的明文："),plain);
    Aes aes256(16,key);


    // 加密，输出密文
    unsigned char* output = (unsigned char*)malloc(16*sizeof(char));
    memset(output,'0',16);
    aes256.Cipher(plain, output); // encipher 16-bit input
    _print4x4Data(QString("加密后的密文："),output);

    //将结果转为QByteArray
    QByteArray res;
    res.resize(16);
    for(int i=0; i<16; ++i)
        res[i]=output[i];
//    qDebug()<<"_getEncryptCode返回的加密后的QByteArray:"<<res;

    return res;
}

//对“有效明文+填充数据”进行解密得明文
QByteArray CUdpDataFrame::_getDecryptCode(QByteArray cryptData)
{
//    qDebug()<<"传入_getDecryptCode的cryptData:"<<cryptData;
//    qDebug()<<"传入_getDecryptCode的cryptData.toHex():"<<cryptData.toHex();

    unsigned char* crypt = (unsigned char*)malloc(16*sizeof(char));
    memset(crypt,'0',16);

    //转存
    for(int i=0; i<16; i++)
        crypt[i]=cryptData.at(i);

//    _print4x4Data(QString("解密前的密文："),crypt);

    Aes aes256(16,key);

    // 解密，输出解密后的明文
    unsigned char* output = (unsigned char*)malloc(16*sizeof(char));
    memset(output,0,16);
    aes256.InvCipher(crypt, output);    //解密
//    _print4x4Data(QString("解密后的明文："),output);

    //将结果转为QByteArray
    QByteArray res;
    res.resize(16);
    for(int i=0; i<16; ++i)
        res[i]=output[i];
//    qDebug()<<"_getDecryptCode返回的解密后的QByteArray:"<<res;

    return  res;
}

//发送帧数据拼接
QByteArray CUdpDataFrame::_sendUdpDataJoint()
{
    QByteArray sendUdpData;
    sendUdpData.append(this->m_headFlag);   //帧起始符
    qDebug()<<"_sendUdpDataJoint(): m_plain_effective_length:"<<this->m_plain_effective_length;
    sendUdpData.append(this->m_effective_length); //有效长度
    sendUdpData.append(this->m_cryptographic_params);   //加密参
    sendUdpData.append(this->m_cryptographic_data); //保密信息密文
    sendUdpData.append(this->m_check_code_crc); //校验码
    sendUdpData.append(this->m_tailFlag);   //结束符

    return sendUdpData;
}

void CUdpDataFrame::_print4x4Data(QString type, unsigned char *data)
{
    QVector<QVector<unsigned char>> result;
    for(int i=0; i<16; ){
        if(i%4==0){
            QVector<unsigned char> vec;
            for(int j=0; j<4; j++)
                vec.push_back(data[i+j]);
            i+=4;
            result.push_back(vec);
        }
    }
    qDebug()<<type;
    for(int i=0; i<4; i++){
        qDebug()<<result[i][0]<<", "<<result[i][1]<<", "<<result[i][2]<<", "<<result[i][3];
    }
}


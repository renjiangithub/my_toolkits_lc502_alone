#ifndef CUDPDATAFRAME_H
#define CUDPDATAFRAME_H

#include <QObject>
#include <iostream>


using namespace std;

class CUdpDataFrame
{
public:
    CUdpDataFrame();
    bool IsValidFrame(QString data);

    QByteArray sendUdpDatagram(QByteArray plain_data);    //对传入的明文data进行加密，校验，拼接等，最终返回完整的加密后的数据帧
    QByteArray getUdpDatagram(QByteArray crypt_data);     //对收到的经加密的data进行解密等，最终返回"有效明文"

public:
    //bool _IsLC502(QString frame);
    //bool _IsValidIp(QString ip);

    void testInitial();//测试用，除了有效数据位之外，其它不清楚的内容，全由0填写

private:
    QByteArray _fill(QByteArray plain_effective_data);   //对有效明文进行填充，使其长度为16的整倍数，满足AES256加密要求
    QByteArray _intToByte(int number);  //int 转 QByteArray： https://blog.csdn.net/ykm0722/article/details/7245623
    QByteArray _getCrcResult(); //计算发送帧的校验码
    QByteArray _uint8ToQByteArray(uint8_t intVar); //将uint8_t转为Qbytearray
    QByteArray _uint16ToQByteArray(uint16_t intVar); //将uint16_t转为Qbytearray
    QByteArray _sendUdpDataSeparator(QByteArray Data);//将拼接好的发送帧用分隔符分离开，方便查看

    QByteArray _getEncryptCode(QByteArray plainData);   //对“有效明文+填充数据”进行明文加密得密文
    QByteArray _getDecryptCode(QByteArray cryptData);   //对“有效明文+填充数据”进行解密得明文

    QByteArray _sendUdpDataJoint(); //发送帧数据拼接
public:
    int m_comm_direction; //0: toolkits发送给lc502的数据帧, 1: lc502发送给toolkits的数据帧
    int m_frame_length;

    QByteArray m_headFlag; //“帧起始符”, 1Byte，LC502项目中固定 0xA5

    QByteArray m_effective_length;  //“有效长度”，2Byte，长度=“加密参数”长度+“保密信息密文”长度

    QByteArray m_cryptographic_type;    //“加密模式1Byte”,未授权前，固定加密模式Ox00，授权后获得新密钥，改为用户加密模式Ox01

    QByteArray m_plain_after_fill;//有效明文+填充数据
    QByteArray m_plain_effective_length;    //"有效明文"长度
    QByteArray m_cryptographic_params; //“加密参数”,19Byte,长度=“加密模式1Byte”+“有效明文长度2Byte”+“IV16字节”
    QString    m_IV_string;    //string形式的IV
    QByteArray m_IV;
    QByteArray m_cryptographic_data;//保密信息密文；由“有效明文”+“填充数据”组成。

    QByteArray m_check_code_crc; //校验码，2Byte,用CRC16校验

    QByteArray m_tailFlag; //“帧结束符”, 1Byte，LC502项目中固定 0x5A

    unsigned char *key;
};

#endif // CUDPDATAFRAME_H

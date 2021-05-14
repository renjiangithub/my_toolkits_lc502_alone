#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <vector>
#include <QObject>
#include <QMessageBox>

#define Num_to_byte(ptr,i,sendframe) sendframe.push_back(*(ptr+i));reverse(sendframe.end() - i, sendframe.end());
#define GET_LAMP_STATUS_NUM 3
#define SET_LAMP_SWITCH_NUM 100
#define SET_LAMP_DIM_NUM 100

// Frame size limit comes from hardware implementation.
// Frame Header
//+-------------+---------------+-------------+
//| Frame Start | Controller ID | Data length |
//| 1 byte      | 8 bytes (MAC) | 4 bytes     |
//+-------------+---------------+-----^-------+
//                                    |
// Frame Body                         |
//+-----------------------------------+------------------------+
//|                    AES Encryption Fields                   |
//+--------------+----------+------------------+---------------+
//| Command Code | Response | Real data length | Padding bytes |
//| 2 bytes      | 1 byte   | 4 bytes          |               |
//+--------------+----------+------------------+---------------+
//
// Frame Postfix
//+---------+-----------+
//| CRC16   | Frame End |
//| 2 bytes | 1 byte    |
//+---------+-----------+
const std::size_t g_smart_ele_addr_length = 6;
const std::size_t g_kmax_frame_length = 1024;
const std::size_t g_kmac_width = 8;	//
const std::size_t g_kstart_end_width = 1;
const std::size_t g_kcrc_width = 2;
const std::size_t g_kcrc_and_end_width = g_kcrc_width + g_kstart_end_width;
// Length of plaintext body without padding bytes
const std::size_t g_klength_width = 4;
const std::size_t g_kheader_width = g_kstart_end_width + g_kmac_width + g_klength_width;
const std::size_t g_kcommand_width = 2;
const std::size_t g_kresponse_width = 1;
const std::size_t g_kreal_body_offset = g_klength_width + g_kcommand_width +
    g_kresponse_width;
const std::size_t g_kmax_body_length = g_kmax_frame_length - g_kheader_width -
    g_kcrc_and_end_width;
const uint8_t g_kframe_start = 0xAA;
const uint8_t g_kframe_end = 0x55;
const std::size_t g_terminal_information_length = 46;

uint64_t ControlIDToHex(uint8_t *id,int length=0);

int CalcByte(int crc, char b);
uint16_t CRC16(char *pBuffer, int length);
int HexCharToInt(char c);
void HexToChar(std::string hexstring, std::vector<uint8_t> &bytes);//将2进制数  转换成char
std::string CBCAESEncryptStr(std::string sKey, std::string sIV, const char *plainText);
std::string CBCAESDecryptStr(std::string sKey, std::string sIV, const char *cipherText);

extern void hex_string_to_bytes(std::string &hex_str,uint8_t *data);

extern void hex_string_to_bytes(const std::string& hex_string, std::vector<uint8_t>& bytes);

extern int SetMessageBox(QMessageBox::Icon msg_box_type, QString title, QString msg);

extern std::vector<uint8_t> CurrentTime();

extern QString _AutoCompleteMac(uint64_t mac);

extern QByteArray _uint16ToQByteArray(uint16_t intVar);

extern QByteArray _uint8ToQByteArray(uint8_t intVar); //将uint8_t转为Qbytearray

extern void _print4x4Data(QString description, unsigned char *data);


#endif // COMMON_H

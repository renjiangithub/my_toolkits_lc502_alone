#include "Common.h"
#include <cast/cast.h>
#include <QDebug>

void hex_string_to_bytes(std::string &hex_str, uint8_t *data)
{
    int i = 0;
    int step = (hex_str.size() % 2) == 0 ? 2 : 1;
      std::size_t pos = 0;
      while (pos < hex_str.size()) {
        int num = std::stoi(hex_str.substr(pos, step), 0, 16);
        data[i] = static_cast<uint8_t>(num);
        pos += step;
        step = 2;
        i++;
      }
}

void hex_string_to_bytes(const std::string& hex_string, std::vector<uint8_t>& bytes) {
  int step = (hex_string.size() % 2) == 0 ? 2 : 1;
  std::size_t pos = 0;
  while (pos < hex_string.size()) {
    int num = std::stoi(hex_string.substr(pos, step), 0, 16);
    bytes.push_back(static_cast<uint8_t>(num));
    pos += step;
    step = 2;
  }
}

int SetMessageBox(QMessageBox::Icon msg_box_type, QString title, QString msg)
{
    QMessageBox box(msg_box_type, title,msg);

    if(msg_box_type != QMessageBox::Warning){
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok, QString("确 定"));
        box.setButtonText(QMessageBox::Cancel, QString("取 消"));
    }
    else
    {
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok, QString("确 定"));
    }

    int button = box.exec();
    if (button == QMessageBox::Ok)
        return 0;
    else
        return 1;
}

std::vector<uint8_t> CurrentTime()
{
    std::vector<uint8_t> time_ustr;
    time_t t = time(NULL);
    struct std::tm *now = localtime(&t);
    time_ustr.push_back((now->tm_year + 1900) >> 8);
    time_ustr.push_back(now->tm_year + 1900);
    time_ustr.push_back(static_cast<uint8_t>(now->tm_mon + 1));
    time_ustr.push_back(static_cast<uint8_t>(now->tm_mday));
    uint8_t dayofweek = static_cast<uint8_t>(now->tm_wday);
    if (dayofweek == 0) {
        dayofweek = 7;  // Sunday == 7 in this protocol
    }
    time_ustr.push_back(dayofweek);
    time_ustr.push_back(static_cast<uint8_t>(now->tm_hour));
    time_ustr.push_back(static_cast<uint8_t>(now->tm_min));
    time_ustr.push_back(static_cast<uint8_t>(now->tm_sec));

    return time_ustr;
}

QString _AutoCompleteMac(uint64_t mac)
{
    std::string addr = qingpei::toolkit::cast::num_to_hex_string(mac);
    QString mac_addr = QString::fromStdString(addr);
    int complete_num = 16 - mac_addr.length();
    for (int i = 0; i < complete_num; i++) {
        mac_addr.insert(0,'0');
    }
    return mac_addr.toUpper();
}

QByteArray _uint16ToQByteArray(uint16_t intVar)
{
    QByteArray array;
    int len_intVar = sizeof(intVar);
    array.resize(len_intVar);
    memcpy(array.data(), &intVar, len_intVar);
    QByteArray result;
    result.append(array.mid(1,1)).append(array.mid(0,1));
    return result;
}

void _print4x4Data(QString description, unsigned char *data)
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
    qDebug()<<description;
    for(int i=0; i<4; i++){
        qDebug()<<result[i][0]<<", "<<result[i][1]<<", "<<result[i][2]<<", "<<result[i][3];
    }
}

QByteArray _uint8ToQByteArray(uint8_t intVar)
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

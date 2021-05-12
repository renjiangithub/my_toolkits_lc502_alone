#ifndef CLC502CONTROLLER_H
#define CLC502CONTROLLER_H

#include "cudpdataframe.h"

#include <QObject>
#include <QVector>
#include <QMap>
#include <QDateTime>

#define CHANNEL_NUM 2

class CLC502Controller:public QObject
{
    Q_OBJECT

public:
    CLC502Controller(QObject *parent = nullptr);

    void DealControllerParamData(CUdpDataFrame data_frame); //处理控制器参数

private:


public:
    QString m_name;//LC502名称

    int     m_comm_status; //LC502授权状态，0：已授权，1：未授权

    /*网络参数*/
    QString m_ip;//ip
    int     m_port; //udp端口
    QString m_mask;//子网掩码
    QString m_gateway;//默认网关
    QString m_loginName;//登录用户名

    //通用寄存器：只读
    QString m_type; //设备类型
    QString m_mac;  //MAC
    QString m_serialNo;  //序列号
    QString m_hardware_version;  //硬件版本号
    QString m_software_version;  //软件版本号
    QString m_status;  //设备状态，正常/异常
    QString m_frame_size;  //帧数据容量
    QString m_run_time_after_powerOn;  //上电后运行时间
    QString m_total_run_time;  //总工作时间
    //通用寄存器：可读可写
    QDateTime m_sys_time;  //系统时间
    QString m_reset_info;  //复位信息


    //通道：只读
    int m_ch_power_status[CHANNEL_NUM];  //开关状态
    int m_ch_now_brightness[CHANNEL_NUM];  //当前亮度
    double m_ch_current[CHANNEL_NUM];  //电流(mA)
    double m_ch_voltage[CHANNEL_NUM];  //电压(mV)
    double m_ch_power_factor[CHANNEL_NUM];  //功率因数
    double m_ch_voltage_frequency[CHANNEL_NUM];  //电压频率(Hz)
    double m_ch_effective_power[CHANNEL_NUM];  //有功功率(W)
    double m_ch_use_power[CHANNEL_NUM];  //总消耗电量(度)
    long long int m_ch_lightOn_time[CHANNEL_NUM];  //总亮灯时间
    double m_ch_current_leak[CHANNEL_NUM];  //漏电流(mA)
    QString m_ch_multicast_no[CHANNEL_NUM];  //组播号
    //通道：可读可写
    int m_ch_adjust_light_value[CHANNEL_NUM]; //开控制亮度
    int     m_ch_adjust_light_switch[CHANNEL_NUM]; //开/关操作: 0:关，1:开
    QString m_ch_adjust_light_feedback[CHANNEL_NUM];  //开关操作的反馈


    //传感器:
    //传感器:只读
    int m_sensor_rdm_status; //RDM传感器状态
    double m_sensor_rdm_temperature; //RDM 温度(℃)
    double m_sensor_rdm_humidity; //RDM 湿度(%Rh)
    int m_sensor_rs_ws_status; //RS-WS传感器状态
    double m_sensor_rs_ws_temperature; //RS-WS温度(℃)
    double m_sensor_rs_ws_humidity; //RS-WS湿度(%Rh)
    int m_sensor_pm25; //PM2.5传感器
    int m_sensor_pressure; //气压(hPa)
    double m_sensor_noise; //噪音(dB)
    double m_sensor_wind_speed; //风速(m/s)
    double m_sensor_wind_direction; //风向
    int m_sensor_infrared_status; //红外传感器状态
    int m_sensor_infrared_trigger; //红外传感器触发
    int m_sensor_di_1; //第1路 DI
    int m_sensor_di_2; //第2路 DI
    int m_sensor_di_3; //第3路 DI
    int m_sensor_di_4; //第4路 DI
    //传感器:读写
    int m_sensor_do_1_status; //第1路DO传感器状态
    int     m_sensor_do_1_switch; //第1路DO传感器开/关操作: 0:关，1:开
    QString m_sensor_do_1_feedback;  //第1路DO传感器开关操作的反馈
    int m_sensor_do_2_status; //第2路DO传感器状态
    int     m_sensor_do_2_switch; //第2路DO传感器开/关操作: 0:关，1:开
    QString m_sensor_do_2_feedback;  //第2路DO传感器开关操作的反馈



};

#endif // CLC502CONTROLLER_H

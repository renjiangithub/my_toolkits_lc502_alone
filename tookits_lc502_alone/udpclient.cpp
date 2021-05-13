#include "udpclient.h"
#include <QDebug>

Udpclient::Udpclient(QObject *parent) : QObject(parent)
{

}

//进行发送
void Udpclient::ExecTask()
{
    qDebug()<<"void Udpclient::ExecTask()";
    Write(m_write_frame.m_final_send_frame, m_write_frame.m_final_send_frame_length);
}

//有正在执行的任务，等待
void Udpclient::_WaitForExecTask()
{
    //TODO
}

void Udpclient::Write(QByteArray m_final_send_frame, int m_final_send_frame_length)
{
     m_has_execting_task = true;
    //TODO:2021年5月13日下班，明天从这里开始写

}

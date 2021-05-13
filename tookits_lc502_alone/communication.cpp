#include "communication.h"

Communication::Communication()
{

}

bool Communication::DeviceOpen()
{
    if(m_CommuType==0)
        return _DeviceOpen_Com();
    else if(m_CommuType==1)
        return _DeviceOpen_Tcp();
    else if(m_CommuType==2)
        return _DeviceOpen_Udp();
    else
        return false;
}

bool Communication::_DeviceOpen_Com()
{
    return true;

}

bool Communication::_DeviceOpen_Tcp()
{
    return true;

}

bool Communication::_DeviceOpen_Udp()
{
    return true;
}

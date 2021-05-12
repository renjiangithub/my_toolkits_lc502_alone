#ifndef MYSTRUCT_H
#define MYSTRUCT_H

#include <QString>

//#include <bits/stl_vector.h>



using namespace std;


struct SUdpProperty
{
    int m_lc502_port;
    int m_receive_port;
    int m_read_timeout;
    QString m_broadcast_use_ip;
};

struct RecvFrame{
    bool m_is_completed;
    //std::vector<uint8_t> m_recv_frame;
};

extern SUdpProperty g_sudp_property;

#endif // MYSTRUCT_H

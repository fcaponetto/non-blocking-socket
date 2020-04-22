#ifndef _PACKET_H_
#define _PACKET_H_

namespace packet
{
    typedef struct
    {
        float foo[2];
        float bar[2];
    }client2server;

    typedef struct
    {
        float fobar[2];
        int abc;
    }server2client;
}

#endif //_PACKET_H_
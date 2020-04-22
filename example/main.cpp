#include <UdpSocket.h>
#include "Packet.h"
#include <iostream>

int main()
{
    sock::UdpSocket<packet::client2server, packet::server2client> udpSocket_;
    packet::client2server pkt_client2server_;
    packet::server2client pkt_server2client_;

    udpSocket_.sock_init();
    udpSocket_.sock_connect("127.0.0.1", 4124);

    while(true)
    {

        pkt_client2server_.bar[0] = 0;
        pkt_client2server_.bar[1] = 0;
        pkt_client2server_.foo[0] = 1;
        pkt_client2server_.foo[1] = 1;
        udpSocket_.sock_send(pkt_client2server_);

        std::cout<< "Sent..." << std::endl;

        // Non blocking call - sock_receive() returns immediately
        if(udpSocket_.sock_receive(pkt_server2client_) > 0)
        {
            std::cout << 
            pkt_server2client_.fobar[0] << "\n" <<
            pkt_server2client_.fobar[1] << "\n" <<
            pkt_server2client_.abc <<
            std::endl;
        }
        sleep(1);
    }
} 
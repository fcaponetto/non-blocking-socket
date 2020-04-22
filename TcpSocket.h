/*
 * Author: Fernando Caponetto
 * email:  fernando.caponetto@iit.it
*/

#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include "ISocket.h"

namespace sock
{

template<typename S, typename R = S>
class TcpSocket : public ISocket<S,R>
{
public:

    TcpSocket() = default;

    void sock_init() override
    {
        /*
		* create socket
		*/
        if ((this->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
        {
            throw std::runtime_error("Could not create TCP socket");
        }
    }

};

}

#endif //_TCP_SOCKET_H_

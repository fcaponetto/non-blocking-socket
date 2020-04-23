/*
 * Author: Fernando Caponetto
 * email:  fernando.caponetto@iit.it
*/

#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_

#include "ISocket.h"

namespace sock
{

template<typename S, typename R = S>
class UdpSocket : public ISocket<S,R>
{
public:

	void sock_init() override
	{

#ifdef _WIN32
		/* Initialize Winsock */
		// consoleLog->info("Initialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			WSACleanup();
			// exit(EXIT_FAILURE);
		}
#endif

		/*
		* create socket
		*/
		if ((this->socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			perror("socket() udp");
			throw std::runtime_error("socket() UDP");
		}

// set socket timeout option
#ifdef __COBALT__
//        // This socket control option is used to specify the time-out on the socket before it returns.
//        // It is used typically to wait for data on a Read.
//        // The time-out specifies the amount of time the function will wait for data before it returns.
//        int64_t timeout_ns = 250;
//        if ( ioctl ( this->socket_, RTNET_RTIOC_TIMEOUT, &timeout_ns ) < 0 )
//            DPRINTF ( "ioctl RTNET_RTIOC_TIMEOUT failed\n" );
#elif __linux__
		int opt = 1;
		ioctl(this->socket_, FIONBIO, &opt); /* Non-blocking */

//		int flags = fcntl(this->socket_, F_GETFL, 0);
//		fcntl(this->socket_, F_SETFL, flags | O_NONBLOCK); /* Non-blocking */
#endif

    }

    void set_recv_buf(size_t& sendBuff)
    {
        socklen_t optlen = sizeof(int);
        size_t sendbuff;

        if ( setsockopt( this->socket_, SOL_SOCKET, SO_RCVBUF, &sendbuff, sizeof(sendbuff) ) == SOCKET_ERROR )
        {
            throw std::runtime_error("error setsockopt()");

        }

        // Get buffer size
        auto res = getsockopt(this->socket_, SOL_SOCKET, SO_RCVBUF, &sendbuff, &optlen);

        if(res == -1)
            throw std::runtime_error("error getsockopt()");
        else
            printf("revc buffer size = %d\n", sendbuff);
    }
};

}

#endif // _UDP_SOCKET_H_


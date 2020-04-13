/*
 * Author: Fernando Caponetto
 * email:  fernando.caponetto@iit.it

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#ifndef _ISOCKET_H_
#define _ISOCKET_H_

#include <memory>
#include <string.h>     /* Commonly used string-handling functions */

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") // Winsock Library
#elif __linux__
#include <unistd.h>     /* Prototypes for many system calls */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <fcntl.h>      /* Non blocking socket */

#define SOCKET int
#define SOCKET_ERROR (SOCKET)(-1)
#define NO_ERROR 0

#endif

#ifdef __COBALT__
#include <asm/ioctl.h>
#include <rtdm/rtdm.h>
//#include <rtnet.h>
#define RTIOC_TYPE_NETWORK      RTDM_CLASS_NETWORK
#define RTNET_RTIOC_TIMEOUT     _IOW(RTIOC_TYPE_NETWORK,  0x11, int64_t)
#endif

namespace sock
{
template<typename S, typename R = S>
class ISocket 
{
public:

    typedef std::unique_ptr<ISocket> Ptr;

    virtual void sock_init() = 0;

    virtual ~ISocket() {
#ifdef _WIN32
        closesocket(socket_);
#elif __linux__
        close(socket_);
#endif
    }

    /**
     * Bind the socket for incoming data
     * @param address Local address
     * @param port Local port
     */
    void sock_bind(const char *address, uint16_t port) {
        /* zero out structure */
        memset((char *) (&local_), 0, sizeof(local_));

        /* initialize address to bind */
        local_.sin_family = AF_INET;
        local_.sin_port = htons(port);
        local_.sin_addr.s_addr = inet_addr(address);

        /* bind socket to address and port */
        if (bind(socket_, (struct sockaddr *) (&local_), sizeof(local_)) != NO_ERROR) {
#ifdef _WIN32
            closesocket(socket_);
#elif __linux__
//			consoleLog->error("bind failed with error: {}", errno);
            close(socket_);
#endif
            throw std::runtime_error("bind() failed");
        }
    }

    /**
     * Connect to the destination socket
     * @param address Destination address
     * @param port Destination port
     */
    void sock_connect(const char *address, uint16_t port) {
        memset((char *) &remote_, 0, sizeof(remote_));
        remote_.sin_family = AF_INET;
        remote_.sin_port = htons(port);
        remote_.sin_addr.s_addr = inet_addr(address);

        if (connect(socket_, (struct sockaddr *) (&remote_), sizeof(remote_)) < 0) {
#ifdef _WIN32
            closesocket(socket_);
#elif __linux__
            close(socket_);
#endif
            throw std::runtime_error("connect() failed");
        }
    }

    /**
     * Send bytes to connected socket
     * @param tx structure to be sent
     * @param size number of bytes to copy into tx
     */
    template<typename Sock_Tx_Types>
    int sock_send(Sock_Tx_Types &tx, const size_t size = 0) {
        if (size) {
            size_sender_ = size;
        }

        memcpy(buffer_sender_.get(), &tx, size_sender_);

        int bytes =
                send(socket_,                // Connected socket
                        buffer_sender_.get(),    // Data buffer
                        size_sender_,            // Length of data
                        0);

        return bytes;
    }

    /**
     * Receive bytes to connected socket
     * @param rx structure to be filled
     * @param size number of bytes to copy into rx
     */
    template<typename Sock_Rx_Types>
    int sock_receive(Sock_Rx_Types &rx, size_t size = 0) {

        if (size) {
            size_receiver_ = size;
        }

#ifdef _WIN32
        int si_len = sizeof(si_recv);
#elif __linux__
        socklen_t si_len = sizeof(si_recv);
#endif

        /* If there is nothing in the buffer, EAGAIN will be raised. It might happen */
        ssize_t bytes =
                recv(socket_,           // Bound socket
                        buffer_receiver_.get(), // Data buffer
                        size_receiver_,         // Length of data
                        0);

//        if (errno == EAGAIN)
//        {
//            ret_value = -1;
//        }
//        perror("errno: ");

        if (bytes > 0)
            memcpy(&rx, buffer_receiver_.get(), size_receiver_);

        return bytes;
    }

    void set_recv_buf(size_t sendBuff)
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

protected:

    ISocket()
    {
        size_receiver_ = sizeof(R);
        buffer_receiver_ = std::make_unique<char[]>(size_receiver_);
        // if(S != R)
        // {
        size_sender_ = sizeof(S);
        buffer_sender_ = std::make_unique<char[]>(size_sender_);
        // }
    }

    std::unique_ptr<char[]> buffer_sender_;
    std::unique_ptr<char[]> buffer_receiver_;

    size_t size_receiver_;

    size_t size_sender_;

#ifdef _WIN32
    WSADATA wsaData;
#endif

    SOCKET socket_;
    struct sockaddr_in local_, remote_, si_recv;

};
}
#endif //_ISOCKET_H_

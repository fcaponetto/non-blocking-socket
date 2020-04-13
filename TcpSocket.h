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

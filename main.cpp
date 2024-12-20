/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/20 10:35:10 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"
#include "Connection.hpp"
#include "EpollWrapper.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h> 
#include <sys/epoll.h>

int main() {
    try {
        Listener listener(8080);
        EpollWrapper epollWrapper(100);
        epollWrapper.addEvent(listener.getFd());
        std::map<int, Connection*> connections;
        while (true) {
            int nfds = epollWrapper.epwait();
            for (int i = 0; i < nfds; ++i) {
                int eventFd = epollWrapper.getEventsList()[i].data.fd;
                uint32_t ev = epollWrapper.getEventsList()[i].events;
                if (eventFd == listener.getFd()) {
                    try {
                        Connection* newConn = new Connection(listener.getFd());
                        connections[newConn->getFd()] = newConn;
                        epollWrapper.addEvent(newConn->getFd());
                    } catch (const std::exception &e) {
                        std::cerr << "Accept failed: " << e.what() << std::endl;
                    }
                } else if(connections.find(eventFd) != connections.end()) {
                    Connection* conn = it->second;
                    if (ev & EPOLLIN) {
                        try {
                            conn->readSocket();
                            ev = EPOLLOUT;
                        } catch (const std::runtime_error &re) {
                            std::cout << "Read error: " << re.what() << std::endl;
                        }
                    } else if (ev & EPOLLOUT) {
                        try {
                            conn->writeSocket();
                        } catch (const std::runtime_error &re) {
                            std::cout << "Write error: " << re.what() << std::endl;
                        }
                    }
                    if (ev & (EPOLLERR | EPOLLHUP)) {
                        // コネクションを閉じる
                        std::cerr << "Error or hangup on connection fd=" << eventFd << std::endl;
                        close(conn->getFd());
                        delete conn;
                        connections.erase(it);
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << " (" << strerror(errno) << ")" << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}


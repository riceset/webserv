/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/20 18:51:06 by rmatsuba         ###   ########.fr       */
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
                    if (ev & EPOLLIN) {
                        try {
                            connections[eventFd]->second->readSocket();
                            ev = EPOLLOUT;
                        } catch (const std::runtime_error &re) {
                            epollWrapper.deleteEvent(eventFd);
                            close(connections[eventFd]->getFd());
                            delete connections[eventFd]->second;
                            std::cout << "Read error: " << re.what() << std::endl;
                        }
                    } else if (ev & EPOLLOUT) {
                        try {
                            connections[eventFd]->second->writeSocket();
                            ev = EPOLLIN;
                        } catch (const std::runtime_error &re) {
                            epollWrapper.deleteEvent(eventFd);
                            close(connections[eventFd]->getFd());
                            delete connections[eventFd]->second;
                            std::cout << "Write error: " << re.what() << std::endl;
                        }
                    } else if (ev & (EPOLLERR | EPOLLHUP)) {
                        // コネクションを閉じる
                        std::cerr << "Error or hangup on connection fd=" << eventFd << std::endl;
                        epollWrapper.deleteEvent(eventFd);
                        close(connections[eventFd]->getFd());
                        delete connections[eventFd];
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


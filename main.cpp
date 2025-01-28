/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/27 20:41:30 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"
#include "Connection.hpp"
#include "EpollWrapper.hpp"
#include "ConnectionWrapper.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h> 
#include <sys/epoll.h>
#include <algorithm>

int main() {
    try {
        /* call config process */
        Listener listener(8080);
        Listener listener2(8081);
        EpollWrapper epollWrapper(100);
        epollWrapper.addEvent(listener.getFd());
        ConnectionWrapper connections;
        while (true) {
            int nfds = epollWrapper.epwait();
            for (int i = 0; i < nfds; ++i) {
                struct epoll_event current_event = epollWrapper[i];
                std::cout << "Event on fd=" << current_event.data.fd << std::endl;
                if (current_event.data.fd == listener.getFd()) {
                    std::cout << "Accepting connection" << std::endl;
                    try {
                        Connection* newConn = new Connection(listener.getFd());
                        epollWrapper.addEvent(newConn->getFd());
                        connections.addConnection(newConn);
                    } catch (const std::exception &e) {
                        std::cerr << "Accept failed: " << e.what() << std::endl;
                    }
                } else {
                    Connection* conn = connections.getConnection(current_event.data.fd);
                    if (!conn) {
                        std::cerr << "Connection not found" << std::endl;
                        continue;
                    }
                    if (current_event.events & EPOLLIN) {
                        std::cout << "Reading from connection fd = " << current_event.data.fd << std::endl;
                        try {
                            conn->readSocket();
                            epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
                            std::cout << "Completed reading from connection fd = " << current_event.data.fd << std::endl;
                        } catch (const std::runtime_error &re) {
                            epollWrapper.deleteEvent(current_event.data.fd);
                            connections.removeConnection(current_event.data.fd);
                            std::cout << "Read error: " << re.what() << std::endl;
                        }
                    } else if (current_event.events & EPOLLOUT) {
                            std::cout << "Writing to connection fd = " << current_event.data.fd<< std::endl;
                        try {
                            conn->writeSocket();
                            epollWrapper.setEvent(conn->getFd(), EPOLLIN);
                            std::cout << "Completed writing to connection fd = " << current_event.data.fd<< std::endl;
                        } catch (const std::runtime_error &re) {
                            epollWrapper.deleteEvent(current_event.data.fd);
                            connections.removeConnection(current_event.data.fd);
                            std::cout << "Write error: " << re.what() << std::endl;
                        }
                    } else if (current_event.events & (EPOLLERR | EPOLLHUP)) {
                        std::cerr << "Error or hangup on connection fd=" << current_event.data.fd<< std::endl;
                        epollWrapper.deleteEvent(current_event.data.fd);
                        close(conn->getFd());
                        delete conn;
                    } else {
                        if (conn->isTimedOut()) {
                            std::cerr << "Connection timed out" << std::endl;
                            epollWrapper.deleteEvent(current_event.data.fd);
                            close(conn->getFd());
                            delete conn;
                        }
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


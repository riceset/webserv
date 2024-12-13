/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 18:56:15 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include "Listener.hpp"
#include "Acceptor.hpp"
#include "Epoll.hpp"
#include "EpollEvent.hpp"

int main() {
    int PORT = 8080;
    /* int MAX_EVENTS = 10; */

    /* create listen port */
    Listener listener(PORT);

    /* create epoll */
    Epoll epoll;

    /* add listener to epoll */
    epoll.addEvent(listener.getFd());
    std::cout << "Server started on port " << PORT << std::endl;
    /* stargin the server */
    while (true) {
        int nfds = epoll.epwait();
        if (nfds == -1)
            return 1;
        for (int i = 0; i < nfds; i++) {
            int c_event = epoll.getEventsList()[i];
            if (listener.getFd() == c_event.data.fd) {
                try {
                    Acceptor acceptor(listener.getFd());
                    epoll.addEvent(acceptor.getFd());
                    std::cout << "Accepted connection from " << acceptor.getFd() << std::endl;
                } catch (std::exception &e) {
                    std::cerr << e.what() << std::endl;
                }
            } else {
                try {
                   c_event. 
                }
            }
        }
    }
    return 0;
}


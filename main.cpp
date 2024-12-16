/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/16 10:48:21 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include <map>          // C++98でunordered_mapが使えないのでmapを使用
#include <cstring>
#include <utility>
#include <vector>
#include "Listener.hpp"
#include "Connection.hpp"
#include "Epoll.hpp"
#include <unistd.h>

int main() {
	int port = 8080;
	int max_events = 64;
	Listener listener(port);
	Epoll epoll(max_events);
	std::map<int, Connection> connections;
	epoll.addEvent(listener.getFd());
	while (true) {
		int nfds = epoll.epwait();
		for (int i = 0; i < nfds; i++) {
			int event_fd = epoll.getEventsList()[i].data.fd;
			if (event_fd == listener.getFd()) {
				try {
					Connection new_connection(listener.getFd());
					epoll.addEvent(new_connection.getFd());
					connections.insert(std::make_pair(new_connection.getFd(), new_connection));
					std::cout << "New connection: " << new_connection.getFd() << std::endl;
				} catch (std::runtime_error &e) {
					std::cerr << e.what() << std::endl;
				}
			} else {
				try {
					Connection &connection = connections.at(event_fd);
					connection.read();
					std::string request = connection.getRbuff();
					std::cout << "Received request: " << request << std::endl;
				} catch (std::runtime_error &e) {
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}
	return 0;
} 



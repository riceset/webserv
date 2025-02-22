/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/22 19:16:11 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

#include "Connection.hpp"
#include "ConnectionWrapper.hpp"
#include "EpollWrapper.hpp"
#include "Listener.hpp"

int main()
{
	try
	{
		/* Load configuration */
		std::string confPath = "src/config/sample/test.conf";
		std::ifstream ifs(confPath.c_str());
		if (!ifs) {
		    throw std::runtime_error("Failed to open configuration file");
		}
		std::stringstream buffer;
    	buffer << ifs.rdbuf();
    	std::string content = buffer.str();
		MainConf mainConf(content);
		/* mainConf.debug_print(); */
		/* Make Listener, EpollWrapper, ConnectionWrapper */
		Listener listener(8080);
		EpollWrapper epollWrapper(100);
		ConnectionWrapper connections;
		epollWrapper.addEvent(listener.getFd());

		/* Main loop */
		while(true)
		{
			int nfds = epollWrapper.epwait();
			for(int i = 0; i < nfds; ++i)
			{
				struct epoll_event current_event = epollWrapper[i];
				std::cout << "Event on fd=" << current_event.data.fd
						  << std::endl;
				if(current_event.data.fd == listener.getFd())					// accept処理
				{
					try
					{
						Connection *newConn = new Connection(listener.getFd());
						epollWrapper.addEvent(newConn->getFd());
						connections.addConnection(newConn);
					}
					catch(const std::exception &e)
					{
						std::cerr << "Accept failed: " << e.what() << std::endl;
					}
				}
				else
				{
					Connection *conn =
						connections.getConnection(current_event.data.fd);
					if(!conn)
					{
						std::cerr << "Connection not found" << std::endl;
						continue;
					}
					if(current_event.events & EPOLLIN)							// read処理
					{
						std::cout << "Reading from connection fd = "
								  << current_event.data.fd << std::endl;
						try
						{
							if (conn->readSocket() == true) {
								// todo method の判定を行う(POST, GET, DELETE)
								// CGI と 静的fileの処理を追加 (GET, POSTの場合)
								int new_fd = conn->getTargetFD(); // dynamic file or static file
								epollWrapper.addEvent(new_fd);
								epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
								std::cout
									<< "Completed reading from connection fd = "
									<< current_event.data.fd << std::endl;
							} else {
								std::cout << "Request is not completed" << std::endl;
							}
						}
						catch(const std::runtime_error &re)
						{
							epollWrapper.deleteEvent(current_event.data.fd);
							connections.removeConnection(current_event.data.fd);
							std::cout << "Read error: " << re.what()
									  << std::endl;
						}
					}
					else if(current_event.events & EPOLLOUT)					// write処理
					{
						std::cout << "Writing to connection fd = "
								  << current_event.data.fd << std::endl;
						try
						{
							if (conn->writeSocket(&mainConf) == true) {

								epollWrapper.setEvent(conn->getFd(), EPOLLIN);
								std::cout << "Completed writing to connection fd = "
									  << current_event.data.fd << std::endl;
							} else {
								std::cout << "Response is not completed" << std::endl;
							}
						}
						catch(const std::runtime_error &re)
						{
							epollWrapper.deleteEvent(current_event.data.fd);
							connections.removeConnection(current_event.data.fd);
							std::cout << "Write error: " << re.what()
									  << std::endl;
						}
					}
					else
					{
						if(conn->isTimedOut())
						{
							std::cerr << "Connection timed out" << std::endl;
							epollWrapper.deleteEvent(current_event.data.fd);
							close(conn->getFd());
							delete conn;
						}
					}
				}
			}
		}
	}
	catch(const std::exception &e)
	{
		std::cerr << "Fatal error: " << e.what() << " (" << strerror(errno)
				  << ")" << std::endl;
		return EXIT_FAILURE;
	}
}

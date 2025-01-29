/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/01/29 20:01:24 by rmatsuba         ###   ########.fr       */
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

#include "Connection.hpp"
#include "ConnectionWrapper.hpp"
#include "EpollWrapper.hpp"
#include "Listener.hpp"

int main()
{
	try
	{
		/* Make Listener, EpollWrapper, ConnectionWrapper */
		Listener listener(8080);
		EpollWrapper epollWrapper(100);
		ConnectionWrapper connections;
		epollWrapper.addEvent(listener.getFd());
		/* Main loop */
		while(true)
		{
			/* Get count of file descriptors registered in epollWrapper */
			int nfds = epollWrapper.epwait();
			/* Check event of each file descriptor */
			for(int i = 0; i < nfds; ++i)
			{
				/* Get current event from epollWrapper */
				struct epoll_event current_event = epollWrapper[i];
				std::cout << "Event on fd=" << current_event.data.fd
						  << std::endl;
				/* If event is on listener, accept new connection */
				if(current_event.data.fd == listener.getFd())
				{
					try
					{
						/* Make new socket and accept new connection */
						Connection *newConn = new Connection(listener.getFd());
						/* Registerd new socket to epollWrapper */
						epollWrapper.addEvent(newConn->getFd());
						/* Add new connection to connections */
						connections.addConnection(newConn);
					}
					catch(const std::exception &e)
					{
						std::cerr << "Accept failed: " << e.what() << std::endl;
					}
					/* If event is on Connection, read or write data */
				}
				else
				{
					/* Get current connection from connections */
					Connection *conn =
						connections.getConnection(current_event.data.fd);
					if(!conn)
					{
						std::cerr << "Connection not found" << std::endl;
						continue;
					}
					/* If event is ready to read, read data from connection */
					if(current_event.events & EPOLLIN)
					{
						std::cout << "Reading from connection fd = "
								  << current_event.data.fd << std::endl;
						try
						{
							/* Read Http request */
							conn->readSocket();
							/* Change State to write */
							epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
							std::cout
								<< "Completed reading from connection fd = "
								<< current_event.data.fd << std::endl;
						}
						catch(const std::runtime_error &re)
						{
							epollWrapper.deleteEvent(current_event.data.fd);
							connections.removeConnection(current_event.data.fd);
							std::cout << "Read error: " << re.what()
									  << std::endl;
						}
						/* If event is ready to write, write data to connection
						 */
					}
					else if(current_event.events & EPOLLOUT)
					{
						std::cout << "Writing to connection fd = "
								  << current_event.data.fd << std::endl;
						try
						{
							/* Write Http response */
							conn->writeSocket();
							/* Change State to read */
							epollWrapper.setEvent(conn->getFd(), EPOLLIN);
							std::cout << "Completed writing to connection fd = "
									  << current_event.data.fd << std::endl;
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main copy.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 03:01:38 by atsu             ###   ########.fr       */
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
#include "CGI.hpp"

// header は server で付与する必要がある
void readStaticFile(Connection &conn, EpollWrapper &epollWrapper)
{
	char buff[1024];
	ssize_t rlen = read(conn->getStaticFd(), buff, 1024);

	if (rlen == -1)
	{
		std::cout << "read static failed" << std::endl; // debug
		epollWrapper.deleteEvent(current_event.data.fd);
		close(conn->getStaticFd());
		std::cout << "static file closed" << std::endl; // debug
		return ;
	}
	buff[rlen] = '\0';
	conn->setStaticBuff(buff);
	if (rlen < 1024)
	{
		epollWrapper.deleteEvent(current_event.data.fd);
		close(conn->getStaticFd());
		conn->setHeader();
		std::cout << "static file closed" << std::endl; // debug
		epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
		std::cout
			<< "Completed reading from connection fd = "
			<< current_event.data.fd << std::endl; // debug
	}
	return;
}

// header は CGI で作成する
void readPipe(Connection &conn, EpollWrapper &epollWrapper)
{
	char buff[1024];
	CGI cgi;

	cgi = conn->getCGI();
	ssize_t rlen = read(cgi.getFd(), buff, 1024);
	if (rlen == -1)
	{
		cgi->stopCGI();
		std::cout << "read pipe failed" << std::endl; // debug
		return ;
	}
	buff[rlen] = '\0';
	conn->setWbuff(buff);
	if (rlen < 1024)
	{
		epollWrapper.deleteEvent(current_event.data.fd);
		close(conn->getStaticFd());
		std::cout << "dynamic file closed" << std::endl; // debug
		std::cout
			<< "Completed reading from CGI fd = "
			<< current_event.data.fd << std::endl; // debug
	}
	return;
}

// readした後にすぎにparseして、エラーケースは即座にwrite socketする
void readSocket(Connection &conn, EpollWrapper &epollWrapper, MainConf &mainConf) {
	std::cout << "Reading from connection fd = "
				<< current_event.data.fd << std::endl; // debug
	bool read_success = false;
	try
	{
		read_success = conn->readSocket();
	}
	catch(const std::runtime_error &re)
	{
		epollWrapper.deleteEvent(current_event.data.fd);
		connections.removeConnection(current_event.data.fd);
		std::cout << "Read error: " << re.what()
					<< std::endl;
	}
	if (read_success == true)
	{
		epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
		if (conn->request_->hasError(mainConf) == true) // 404 not found など
		{
			// 404 not found などのエラーを buff につめて返す
			return ;
		}

		switch (conn->request_->getMethod())
		{
			case GET:
				int new_fd = conn->setReadFd(); // dynamic file or static file
				epollWrapper.addEvent(new_fd);
				break;
			case POST:
				// todo POSTの処理
				break;
			case DELETE:
				// todo DELETEの処理
				break;
			default:
				break;
		}
	}
	else
	{
		std::cout << "Request is not completed" << std::endl; // debug
	}
}

void writeSocket(Connection &conn, EpollWrapper &epollWrapper, MainConf &mainConf) {
	// todo もしも wbuff が空の場合は書き込まない
	std::cout << "Writing to connection fd = "
				<< current_event.data.fd << std::endl;
	try
	{
		if (conn->writeSocket(mainConf) == true) {

			epollWrapper.setEvent(conn->getFd(), EPOLLIN);
			std::cout << "Completed writing to connection fd = "
					<< current_event.data.fd << std::endl; // debug
		} else {
			std::cout << "Response is not completed" << std::endl; // debug
		}
	}
	catch(const std::runtime_error &re)
	{
		epollWrapper.deleteEvent(current_event.data.fd);
		connections.removeConnection(current_event.data.fd);
		std::cout << "Write error: " << re.what()
					<< std::endl; // debug
	}
}

std::string getConfContent() {
	std::string confPath = "src/config/sample/test.conf";
	std::ifstream ifs(confPath.c_str());
	if (!ifs) {
		throw std::runtime_error("Failed to open configuration file");
	}
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	std::string content = buffer.str();
	return content;
}

int main()
{
	/* Load configuration */
	std::string content = getConfContent();
	MainConf mainConf(content);

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
			if(current_event.data.fd == listener.getFd())
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
				Connection *conn = connections.getConnection(current_event.data.fd);

				if(!conn)
				{
					std::cerr << "Connection not found" << std::endl;
					continue;
				}
				if(conn->isTimedOut())
				{
					// todo 503 time out の実装が必要
					std::cerr << "Connection timed out" << std::endl;
					if (conn->getStaticFd() != -1)
						close(conn->getStaticFd());
					if (conn->getCGI() != -1)
						conn->getCGI().killCGI();
					epollWrapper.deleteEvent(current_event.data.fd);
					close(conn->getFd());
					delete conn;
					continue;
				}

				FileTypes type = conn->getFdType(current_event.data.fd);
				switch (type) {
					// todo エラーハンドリングを行う
					case STATIC:
						readStaticFile(conn, epollWrapper);
						break;
					case PIPE:
						readPipe(conn, epollWrapper);
						break;
					case SOCKET:
						if (current_event.events & EPOLLIN)
						{
							readSocket(conn, epollWrapper);
						}
						else if (current_event.events & EPOLLOUT)
						{
							writeSocket(conn, epollWrapper);
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

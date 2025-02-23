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

enum FileStatus
{
	SUCCESS,
	ERROR,
	NOT_COMPLETED,
};

FileStatus readStaticFile(Connection &conn)
{
	char buff[1024];
	ssize_t rlen = read(conn.getStaticFd(), buff, 1024);

	if (rlen == -1)
	{
		std::cout << "[main.cpp] read static failed" << std::endl;
		return ERROR;
	}
	buff[rlen] = '\0';
	conn.setStaticBuff(buff);
	if (rlen < 1024)
	{
		// todo ここで header を付与して　wbuff に格納する respoonse を完全に作成する
		std::cout << "[main.cpp] read static file completed" << std::endl;
		return SUCCESS;
	}
	return NOT_COMPLETED;
}

// todo header は CGI で作成する
FileStatus readPipe(Connection &conn)
{
	char buff[1024];
	CGI cgi;

	cgi = conn.getCGI();
	ssize_t rlen = read(cgi.getFd(), buff, 1024);
	if (rlen == -1)
	{
		std::cout << "[main.cpp] read pipe failed" << std::endl;
		cgi.killCGI();
		std::cout << "[main.cpp] kill cgi" << std::endl;
		return ERROR;
	}
	buff[rlen] = '\0';
	conn.setWbuff(buff);
	if (rlen < 1024)
	{
		std::cout << "[main.cpp] read pipe completed" << std::endl;
		// todo ここで header を付与して　wbuff に格納する respoonse を完全に作成する 本来はcgiで作成 しているため、即座に送信するのが望ましい
		return SUCCESS;
	}
	return NOT_COMPLETED;
}

void afterReadSocket(Connection &conn, MainConf *mainConf, EpollWrapper &epollWrapper)
{
	conn.setHttpRequest(mainConf);
	conn.setHttpResponse(); // todo ここで response を作成する(暫定版)

	HttpRequest *request = conn.getRequest();
	HttpResponse *response = conn.getResponse();

	int new_fd;

	if (response->getStatusCode() != 200)
	{
		new_fd = conn.setErrorFd();
		epollWrapper.addEvent(new_fd);
		return;
	}

	switch (request->getMethod())
	{
	case GET:
		new_fd = conn.setReadFd(); // dynamic file or static file
		epollWrapper.addEvent(new_fd);
		return;
	case POST:
		// todo POSTの処理
		return;
	case DELETE:
		// todo DELETEの処理
		return;
	default:
		return;
	}
}

FileStatus readSocket(Connection &conn) {
	std::cout << "[main.cpp] Reading from connection fd = "
				<< conn.getFd() << std::endl; // debug
	bool is_read = false;
	try
	{
		is_read = conn.readSocket();
	}
	catch(const std::runtime_error &re)
	{
		return ERROR;
	}
	if (is_read == true)
	{
		std::cout << "[main.cpp] read connection completed" << std::endl;
		return SUCCESS;
	}
	return NOT_COMPLETED;
}

FileStatus writeSocket(Connection &conn) {
	// todo もしも wbuff が空の場合は書き込まない
	std::cout << "[main.cpp] writing to connection fd = "
			<< conn.getFd() << std::endl; // debug
	bool is_write = false;
	try
	{
		is_write = conn.writeSocket();
	}
	catch (const std::runtime_error &re)
	{
		return ERROR;
	}
	if (is_write == true) {
		std::cout << "[main.cpp] write connection completed" << std::endl;
		return SUCCESS;
	}
	return NOT_COMPLETED;
}

std::string getConfContent() {
	std::string confPath = "src/config/sample/test.conf";
	std::ifstream ifs(confPath.c_str());
	if (!ifs) {
		throw std::runtime_error("[main.cpp] Failed to open configuration file");
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
			std::cout << "[main.cpp] epoll while [" << i << "]" << std::endl;

			struct epoll_event current_event = epollWrapper[i];
			int target_fd = current_event.data.fd;
			std::cout << "[main.cpp] Event on fd=" << target_fd
						<< std::endl;
			if(target_fd == listener.getFd())
			{
				try
				{
					Connection *newConn = new Connection(listener.getFd());
					epollWrapper.addEvent(newConn->getFd());
					connections.addConnection(newConn);
				}
				catch(const std::exception &e)
				{
					std::cerr << "[main.cpp] Accept failed: " << e.what() << std::endl;
				}
			}
			else
			{
				Connection *conn = connections.getConnection(target_fd);

				if(!conn)
				{
					std::cerr << "[main.cpp] Connection not found" << std::endl;
					continue;
				}
				if(conn->isTimedOut())
				{
					// todo 503 time out の実装が必要
					std::cerr << "[main.cpp] Connection timed out" << std::endl;
					if (conn->getStaticFd() != -1)
						close(conn->getStaticFd());
					else if (conn->getCGI().getFd() != -1)
						conn->getCGI().killCGI();
					epollWrapper.deleteEvent(target_fd);
					close(conn->getFd());
					delete conn;
					continue;
				}

				FileTypes type = conn->getFdType(target_fd);
				FileStatus file_status;
				switch (type) {
					// todo エラーハンドリングを行う
					case STATIC:
						file_status = readStaticFile(*conn);
						if (file_status == ERROR)
						{
							epollWrapper.deleteEvent(conn->getFd());
							close(target_fd);
							std::cout << "[main.cpp] static file closed" << std::endl;
							close(conn->getFd());
							std::cout << "[main.cpp] connection closed" << std::endl;
						}
						if (file_status == SUCCESS)
						{
							epollWrapper.deleteEvent(target_fd);
							epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
							close(target_fd);
							std::cout << "[main.cpp] static file closed" << std::endl;
						}
						break;
					case PIPE:
						file_status = readPipe(*conn);
						if (file_status == ERROR)
						{
							epollWrapper.deleteEvent(conn->getFd());
							close(target_fd);
							std::cout << "[main.cpp] dynamic file closed" << std::endl;
							close(conn->getFd());
							std::cout << "[main.cpp] connection closed" << std::endl;
						}
						if (file_status == SUCCESS)
						{
							epollWrapper.deleteEvent(target_fd);
							epollWrapper.setEvent(conn->getFd(), EPOLLOUT); //  本来はSUCCESS後ではなく、cgi 開始した後に書き込むべき time out が発生する可能性があるので悩ましい
							close(target_fd);
							std::cout << "[main.cpp] dynamic file closed" << std::endl;
						}
						break;
					case SOCKET:
						if (current_event.events & EPOLLIN)
						{
							file_status = readSocket(*conn);
							if (file_status == ERROR)
							{
								epollWrapper.deleteEvent(target_fd);
								close(target_fd);
								std::cout << "[main.cpp] connection closed" << std::endl;
							}
							if (file_status == SUCCESS)
							{
								afterReadSocket(*conn, &mainConf, epollWrapper);
								// todo どのタイミングでsocketを書き込むかは処理に委ねられている
							}
						}
						else if (current_event.events & EPOLLOUT)
						{
							file_status = writeSocket(*conn);
							if (file_status == ERROR)
							{
								epollWrapper.deleteEvent(target_fd);
								close(target_fd);
								std::cout << "[main.cpp] connection closed" << std::endl;
							}
							if (file_status == SUCCESS)
							{
								epollWrapper.setEvent(target_fd, EPOLLIN);
								std::cout << "[main.cpp] connection closed" << std::endl;
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

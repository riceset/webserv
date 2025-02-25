/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 13:49:54 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/24 11:44:23by atsu             ###   ########.fr       */
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

// todo current dir が欲しいね

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
		std::cout << "[main.cpp] Error: read static failed fd[" << conn.getStaticFd() << "]" << std::endl;
		return ERROR;
	}
	buff[rlen] = '\0';
	conn.setStaticBuff(buff);
	if (rlen < 1024)
	{
		std::cout << "[main.cpp] read static file completed" << std::endl;
		conn.setHttpResponse();       // response の作成
		conn.setHttpResponseBody();   // body の登録
		conn.setHttpResponseHeader(); // header と start line の登録
		conn.buildResponseString();   // wbuff の登録(writeに備える)
		conn.setStaticFd(-1);
		return SUCCESS;
	}
	return NOT_COMPLETED;
}

void readStaticFiles(ConnectionWrapper &connections, EpollWrapper  &epollWrapper)
{
	std::vector<Connection *> conns = connections.getConnections();
	if (conns.empty())
	{
		return;
	}
	for (std::vector<Connection *>::iterator it = conns.begin(); it != conns.end(); ++it)
	{
		Connection *conn = *it;
		if (conn->getStaticFd() != -1)
		{
			FileStatus file_status = readStaticFile(*conn);
			if (file_status == ERROR)
			{
				close(conn->getStaticFd());
				std::cout << "[main.cpp] Error: static file closed" << std::endl;
				connections.removeConnection(conn->getFd());
				close(conn->getFd());
				std::cout << "[main.cpp] Error: connection closed" << std::endl;
			}
			if (file_status == SUCCESS)
			{
				close(conn->getStaticFd());
				epollWrapper.setEvent(conn->getFd(), EPOLLOUT);
				std::cout << "[main.cpp] static file closed" << std::endl;
			}
		}
	}
}

// todo header は CGI で作成する
FileStatus readPipe(Connection &conn)
{
	char buff[1024];
	CGI *cgi;

	cgi = conn.getCGI();
	ssize_t rlen = read(cgi->getFd(), buff, 1024);
	if (rlen == -1)
	{
		std::cout << "[main.cpp] Error: read pipe failed" << std::endl;
		cgi->killCGI();
		std::cout << "[main.cpp] Error: kill cgi" << std::endl;
		return ERROR;
	}
	buff[rlen] = '\0';
	conn.setWbuff(buff);
	if (rlen < 1024)
	{
		std::cout << "[main.cpp] read pipe completed" << std::endl;
		// todo ここで header を付与して　wbuff に格納する respoonse を完全に作成する 本来はcgiでheaderを作成 しているため、即座に送信するのが望ましい
		conn.setHttpResponse();       // response の作成
		conn.setHttpResponseBody();   // body の登録
		conn.setHttpResponseHeader(); // header と start line の登録
		conn.buildResponseString();   // wbuff の登録(writeに備える)
		return SUCCESS;
	}
	return NOT_COMPLETED;
}

void afterReadSocket(Connection &conn, MainConf *mainConf, EpollWrapper &epollWrapper)
{
	conn.setHttpRequest(mainConf);

	HttpRequest *request = conn.getRequest();

	// todo エラーハンドリングがかなりガバガバ
	if (request->getStatusCode() != 200)
	{
		conn.setErrorFd();
		return;
	}

	CGI * newCGI = new CGI("./www/index.php");
	conn.setCGI(newCGI);
	int new_fd = newCGI->getFd();
	epollWrapper.addEvent(new_fd);

// 	switch (request->getMethod())
// 	{
// 	case GET: // todo 静的ファイルは読み込み完了後に header を付与する
// // ================================================================================================
// 		CGI cgi("./www/index.php");
// 		new_fd = cgi.getFd();
// // ================================================================================================
// 		// conn.setReadFd();
// 		// if (conn.getCGI().getFd() != -1)
// 		// {
// 		// 	new_fd = conn.getCGI().getFd();
// 		// 	std::cout << "[main.cpp] cgi fd = " << new_fd << std::endl;
// 		// 	epollWrapper.addEvent(new_fd);
// 		// }
// 		epollWrapper.addEvent(new_fd);

// 		return;
// 	case POST:
// 		conn.setHttpResponse(); // todo ここで response を作成する body 不要?
// 		// todo POSTの処理
// 		return;
// 	case DELETE:
// 		conn.setHttpResponse(); // todo ここで response を作成する body 不要
// 		// todo DELETEの処理
// 		return;
// 	default:
// 		return;
// 	}
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

		// 空いているfdを片っ端からちょっとづつ読み込む
		readStaticFiles(connections, epollWrapper);

		for(int i = 0; i < nfds; ++i)
		{
			std::cout << std::endl << "[main.cpp] epoll for-loop ===[" << i << "]===" << std::endl;

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
					std::cerr << "[main.cpp] Error: Accept failed: " << e.what() << std::endl;
				}
			}
			else
			{
				Connection *conn = connections.getConnection(target_fd);

				if(!conn)
				{
					std::cerr << "[main.cpp] Error: Connection not found" << std::endl;
					continue;
				}
				if(conn->isTimedOut())
				{
					// todo 503 time out の実装が必要
					std::cerr << "[main.cpp] Connection timed out" << std::endl;
					if (conn->getStaticFd() != -1)
					{
						close(conn->getStaticFd());
					}
					else if (conn->getCGI() != NULL && conn->getCGI()->getFd() != -1)
					{
						conn->getCGI()->killCGI();
						conn->setCGI(NULL);
					}
					epollWrapper.deleteEvent(target_fd);
					connections.removeConnection(target_fd);
					// todo 終了処理はleakが起きると思う
					continue;
				}

				FileTypes type = conn->getFdType(target_fd);
				FileStatus file_status;
				switch (type) {
					// todo エラーハンドリングを行う
					case PIPE:
						std::cout << "[main.cpp] pipe event" << std::endl;
						file_status = readPipe(*conn);
						if (file_status == ERROR)
						{
							epollWrapper.deleteEvent(conn->getFd());
							close(target_fd);
							std::cout << "[main.cpp] dynamic(cgi) file closed" << std::endl;
							close(conn->getFd());
							std::cout << "[main.cpp] connection closed" << std::endl;
						}
						if (file_status == SUCCESS)
						{
							epollWrapper.deleteEvent(target_fd);
							epollWrapper.setEvent(conn->getFd(), EPOLLOUT); //  本来はSUCCESS後ではなく、cgi 開始した後に書き込むべき time out が発生する可能性があるので悩ましい
							close(target_fd);
							std::cout << "[main.cpp] dynamic(cgi) file closed" << std::endl;
						}
						break;
					case SOCKET:
						if (current_event.events & EPOLLIN)
						{
							std::cout << "[main.cpp] socket EPOLLIN (read) event" << std::endl;
							file_status = readSocket(*conn);
							if (file_status == ERROR)
							{
								epollWrapper.deleteEvent(target_fd);
								close(target_fd);
								std::cout << "[main.cpp] Error: connection closed" << std::endl;
							}
							if (file_status == SUCCESS)
							{
								afterReadSocket(*conn, &mainConf, epollWrapper);
								// todo どのタイミングでsocketを書き込むかは処理に委ねられている
							}
						}
						else if (current_event.events & EPOLLOUT)
						{
							std::cout << "[main.cpp] socket EPOLLOUT (write) event" << std::endl;
							file_status = writeSocket(*conn);
							if (file_status == ERROR)
							{
								epollWrapper.deleteEvent(target_fd);
								close(target_fd);
								std::cout << "[main.cpp] Error: connection closed" << std::endl;
							}
							if (file_status == SUCCESS)
							{
								epollWrapper.setEvent(target_fd, EPOLLIN);
								conn->clearValue();
								std::cout << "[main.cpp] connection status cleared and closed" << std::endl;
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

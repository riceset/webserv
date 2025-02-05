/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:25:14 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/02 18:32:53 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <ctime>
#include <iostream>
#include <stdexcept>

#include "ASocket.hpp"

std::time_t Connection::timeout_ = 60;

Connection::Connection() : ASocket() {}

/* Connection constructor */
Connection::Connection(int listenerFd) : ASocket()
{
	/* make a new socket for the client */
	socklen_t len = sizeof(addr_);
	fd_ = accept(listenerFd, (struct sockaddr *)&addr_, &len);
	if(fd_ == -1)
		throw std::runtime_error("[Connection] Accept failed");
	if(fcntl(fd_, F_SETFL, O_NONBLOCK))
		throw std::runtime_error(
			"[Connection] Failed to set socket to non-blocking");
	request_ = NULL;
	response_ = NULL;
	lastActive_ = std::time(NULL);
	std::cout << "Accepted connection from " << addr_.sin_port << std::endl;
}

Connection::~Connection()
{
	close(fd_);
}

int Connection::getFd() const
{
	return fd_;
}

/* Check whether the connection is timed out */
bool Connection::isTimedOut()
{
	std::time_t now = std::time(NULL);
	if(now - lastActive_ > timeout_)
		return true;
	lastActive_ = now;
	return false;
}

/* Reading Http request from the socket */
void Connection::readSocket()
{
	char buff[1024];
	ssize_t rlen = recv(fd_, buff, sizeof(buff) - 1, 0);
	if(rlen < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return;
		}
		throw std::runtime_error("[Connection] Recive failed");
	}
	else if(rlen == 0)
	{
		if(isTimedOut())
			throw std::runtime_error(
				"[Connection] Timed out, Connection closed by client");
		else
			throw std::runtime_error(
				"[Connection] Connection closed by client");
	}
	buff[rlen] = '\0';
	std::cout << "buff: " << buff << std::endl; // デバッグ用
	rbuff_ += buff;
	request_ = new HttpRequest(rbuff_);
}

/* Writing Http response to the socket */
void Connection::writeSocket()
{
	if(!request_)
	{
		throw std::runtime_error("[Connection] No request found");
	}
	try
	{
		response_ = new HttpResponse(request_);
		buildResponseString();
		ssize_t wlen = send(fd_, wbuff_.c_str(), wbuff_.size(), 0);
		if(wlen == -1)
			throw std::runtime_error("[connection] Send failed");
		/* remove Http request instance from connection */
		if(request_)
			delete request_;
		request_ = NULL;
		/* remove Http response instance from connection */
		delete response_;
		response_ = NULL;
		/* remove the data that was sent */
		wbuff_.erase(0, wlen);
	}
	catch(const std::exception &e)
	{
		if(response_)
		{
			delete response_;
			response_ = NULL;
		}
		throw;
	}
}

std::string Connection::getRbuff() const
{
	return rbuff_;
}

std::string Connection::getWbuff() const
{
	return wbuff_;
}

HttpRequest *Connection::getRequest() const
{
	return request_;
}

/* Connection *getConnection(std::vector<Connection *> &connections, int fd) {
 */
/*     std::cout << "Searching for connection with fd=" << fd << std::endl; */
/*     for (unsigned int i = 0; i < connections.size(); i++) { */
/*         if (connections[i]->getFd() == fd) { */
/*             std::cout << "Connection number " << i << std::endl; */
/*             return connections[i]; */
/*         } */
/*     } */
/*     std::cerr << "Connection not found" << std::endl; */
/*     return NULL; */
/* } */

/* Build Http response string */
void Connection::buildResponseString()
{
	std::string startLine = vecToString(response_->getStartLine());
	std::string header = mapToString(response_->getHeader());
	std::string body = response_->getBody();
	wbuff_ = startLine + "\r\n" + header + "\r\n" + body;
}

/* Convert vector to string */
std::string vecToString(std::vector<std::string> vec)
{
	std::string str;
	for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end();
		++it)
	{
		str += *it;
		str += " ";
	}
	return str;
}

/* Convert map to string */
std::string mapToString(std::map<std::string, std::string> mapdata)
{
	std::string str;
	for(std::map<std::string, std::string>::iterator it = mapdata.begin();
		it != mapdata.end();
		++it)
	{
		str += it->first;
		str += ": ";
		str += it->second;
		str += "\r\n";
	}
	return str;
}

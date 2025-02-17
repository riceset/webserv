/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:25:14 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/16 19:48:28 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <functional>

#include "ASocket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "MainConf.hpp"

std::time_t Connection::timeout_ = 60;

Connection::Connection() : ASocket() {}

/* Connection constructor */
Connection::Connection(int listenerFd) : ASocket()
{
	/* make a new socket for the client */
	socklen_t len = sizeof(addr_);
	fd_ = accept(listenerFd, (struct sockaddr *)&addr_, &len);
	if(fd_ == -1)
		throw std::runtime_error("accept failed");
	if(fcntl(fd_, F_SETFL, O_NONBLOCK))
		throw std::runtime_error("Failed to set socket to non-blocking");
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
	std::cout << "now: " << now << std::endl;
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
		throw std::runtime_error("recv failed");
	else if(rlen == 0)
		throw std::runtime_error("connection is closed by client");
	else if (rlen < 1023) {
		/* std::cout << "socket size is less than 1024" << std::endl; */
		buff[rlen] = '\0';
		rbuff_ += buff;
		request_ = new HttpRequest(rbuff_);
	} else {
		/* if recv is rlen is 1023, it means that the buffer is full */
		rbuff_ += buff;
	}
	/* std::cout << "buff: " << buff << std::endl; // デバッグ用 */
}

/* Writing Http response to the socket */
void Connection::writeSocket(MainConf *mainConf)
{
	char buff[1024];
	if(!request_)
	{
		throw std::runtime_error("No request found");
	}
	if (response_ == NULL) {
		response_ = new HttpResponse(request_, mainConf);
		buildResponseString();
		std::cout << "Http Response is not created yet, let's create!!" << std::endl;
	}
	std::size_t copy_len = std::min(wbuff_.size(), static_cast<std::size_t>(1024));
	std::memcpy(buff, wbuff_.data(), copy_len);
	if (copy_len != 1024)
		buff[copy_len] = '\0';
	wbuff_.erase(0, copy_len);
	ssize_t wlen = send(fd_, buff, sizeof(buff), 0);
	std::cout << "wlen: " << wlen << std::endl;
	if (wlen == -1)
			throw std::runtime_error("send failed");
	if (wlen < 1024) {
		delete response_;
		delete request_;
		response_ = NULL;
		request_ = NULL;
		std::cout << "http process is done, delete current request and response" << std::endl;
	}
	/* try */
	/* { */
	/* 	response_ = new HttpResponse(request_, mainConf); */
	/* 	buildResponseString(); */
	/* 	/1* std::cout << "wbuff_: " << wbuff_ << std::endl; *1/ */
	/* 	ssize_t wlen = send(fd_, wbuff_.c_str(), wbuff_.size(), 0); */
	/* 	if(wlen == -1) */
	/* 		throw std::runtime_error("send failed"); */
	/* 	/1* remove Http request instance from connection *1/ */
	/* 	if(request_) */
	/* 		delete request_; */
	/* 	request_ = NULL; */
	/* 	/1* remove Http response instance from connection *1/ */
	/* 	delete response_; */
	/* 	response_ = NULL; */
	/* 	/1* remove the data that was sent *1/ */
	/* 	wbuff_.erase(0, wlen); */
	/* } */
	/* catch(const std::exception &e) */
	/* { */
	/* 	if(response_) */
	/* 	{ */
	/* 		delete response_; */
	/* 		response_ = NULL; */
	/* 	} */
	/* 	throw; */
	/* } */
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:25:14 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 02:34:16 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

std::time_t Connection::timeout_ = 60;

// ==================================== constructor and destructor ====================================

Connection::Connection() : ASocket() {}

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

Connection::Connection(const Connection &other) : ASocket(other)
{
	fd_ = other.fd_;
	static_fd_ = other.static_fd_;
	cgi_ = other.cgi_;
	rbuff_ = other.rbuff_;
	wbuff_ = other.wbuff_;
	static_file_buff_ = other.static_file_buff_;
	request_ = other.request_;
	response_ = other.response_;
	lastActive_ = other.lastActive_;
}

Connection::~Connection()
{
	close(fd_);
}

// ==================================== getter ====================================

int Connection::getFd() const
{
	return fd_;
}

int Connection::getStaticFd() const
{
	return static_fd_;
}

CGI Connection::getCGI() const
{
	return cgi_;
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

FileTypes Connection::getFdType(int fd) const
{
	if (fd == static_fd_)
		return FileTypes::STATIC;
	else if (fd == cgi_.getFd())
		return FileTypes::PIPE;
	else
		return FileTypes::SOCKET;
}

// ==================================== setter ====================================

// readした後にparseして fdを返却する
int Connection::setReadFd()
{
	std::string location_path = request_->getLocationPath();
	int path_size = location_path.size();	
	int fd;

	// cgiかどうか
	if (path_size > 4 && location_path.substr(path_size - 4, 4) == ".php") {
		CGI cgi(location_path);
		// ! エラーハンドリング
		cgi_ = cgi;
		fd = cgi.getFd();
		return fd;
	} else {
		fd = open(location_path.c_str(), O_RDONLY);
		fd_ = fd;
		// ! エラーハンドリング
		return fd;	
	}
}

void Connection::buildResponseString()
{
	std::string startLine = vecToString(response_->getStartLine());
	std::string header = mapToString(response_->getHeader());
	std::string body = response_->getBody(); // body 部分は他で生成されると言う認識
	wbuff_ = startLine + "\r\n" + header + "\r\n" + wbuff_;
}

void Connection::setWbuff(std::string wbuff)
{
	wbuff_ += wbuff;
}

void Connection::setStaticBuff(std::string static_buff)
{
	static_file_buff_ += static_buff;
}


// ==================================== check ==============================================

bool Connection::isTimedOut()
{
	std::time_t now = std::time(NULL);
	std::cout << "now: " << now << std::endl;
	if(now - lastActive_ > timeout_)
		return true;
	lastActive_ = now;
	return false;
}

// ==================================== read and write ====================================

bool Connection::readSocket()
{
	char buff[1024];
	ssize_t rlen = recv(fd_, buff, sizeof(buff) - 1, 0);
	bool is_request_end = false;

	if(rlen < 0)
		throw std::runtime_error("recv failed");
	else if(rlen == 0)
		throw std::runtime_error("connection is closed by client"); // ? ちょうど読み切った場合は0になるのではないか？
	else if (rlen < 1023) {
		buff[rlen] = '\0';
		rbuff_ += buff;
		request_ = new HttpRequest(rbuff_);
		is_request_end = true;
	} else {
		rbuff_ += buff;
	}

	return is_request_end;
}

bool Connection::writeSocket(MainConf *mainConf)
{
	char buff[1024];
	bool is_response_end = false;

	if(!request_)
	{
		throw std::runtime_error("No request found");
	}
	// write socket する前に header は欲しいと思う
	if (response_ == NULL) {
		response_ = new HttpResponse(request_, mainConf);
		buildResponseString();
		std::cout << "wbuff_: " << wbuff_ << std::endl; // debug
		std::cout << "Http Response is not created yet, let's create!!" << std::endl; // debug
	}

	std::size_t copy_len = std::min(wbuff_.size(), static_cast<std::size_t>(1024));
	std::memcpy(buff, wbuff_.data(), copy_len);
	if (copy_len != 1024)
		buff[copy_len] = '\0';
	wbuff_.erase(0, copy_len);
	ssize_t wlen = send(fd_, buff, copy_len, 0);
	std::cout << "wlen: " << wlen << std::endl; // debug
	if (wlen == -1)
			throw std::runtime_error("send failed");
	if (wlen < 1024) {
		delete response_;
		delete request_;
		response_ = NULL;
		request_ = NULL;
		std::cout << "http process is done, delete current request and response" << std::endl;
		is_response_end = true;
	}
	return is_response_end;
}

// ==================================== utils ====================================

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

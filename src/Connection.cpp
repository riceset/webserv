/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:25:14 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/19 22:51:45 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "ASocket.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <errno.h>

Connection::Connection() : ASocket() {}

Connection::Connection(int listenerFd) : ASocket() {
    /* make a new socket for the client */
    socklen_t len = sizeof(addr_);
    fd_ = accept(listenerFd, (struct sockaddr *)&addr_, &len);
    if (fd_ == -1)
        throw std::runtime_error("accept failed");
    if (fcntl(fd_, F_SETFL, O_NONBLOCK))
        throw std::runtime_error("Failed to set socket to non-blocking");
    request_ = NULL;
    respose_ = NULL;
    std::cout << "Accepted connection from " << addr_.sin_port << std::endl;
}

Connection::~Connection() {
    if (request_)
        delete request_;
    /* close(fd_); */
}

int Connection::getFd() const {
    return fd_;
}

void Connection::readSocket() {
    char buff[1024];
    /* read from the client */
    ssize_t rlen = recv(fd_, buff, sizeof(buff) - 1, 0);
    if (rlen < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
        throw std::runtime_error("recv failed");
    } else if (rlen == 0) {
        throw std::runtime_error("client disconnected");
    }
    buff[rlen] = '\0';
    rbuff_ += buff;
    request_ = new HttpRequest(rbuff_);
}

void Connection::writeSocket() {
    /* write to the client */
    ssize_t wlen = send(fd_, wbuff_.c_str(), wbuff_.size(), 0);
    if (wlen == -1)
        throw std::runtime_error("send failed");
    /* remove the data that was sent */
    wbuff_.erase(0, wlen);
}

std::string Connection::getRbuff() const {
    return rbuff_;
}

std::string Connection::getWbuff() const {
    return wbuff_;
}

HttpRequest *Connection::getRequest() const {
    return request_;
}

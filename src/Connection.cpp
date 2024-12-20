/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:25:14 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/20 19:05:44 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "ASocket.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <ctime>

std::time_t Connection::timeout_ = 60;

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
    lastActive_ = std::time(NULL);
    std::cout << "Accepted connection from " << addr_.sin_port << std::endl;
}

Connection::~Connection() {
    /* close(fd_); */
}

int Connection::getFd() const {
    return fd_;
}

bool Connection::isTimedOut() {
    std::time_t now = std::time(NULL);
    if (now - lastActive_ > timeout_)
        return true;
    lastActive_ = now;
    return false;
}

void Connection::readSocket() {
    if (isTimedOut())
        throw std::runtime_error("Connection timed out");
    /* read from the client */
    char buff[1024];
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
    response_ = new HttpResponse(request_);
    /* write to the client */
    ssize_t wlen = send(fd_, wbuff_.c_str(), wbuff_.size(), 0);
    if (wlen == -1)
        throw std::runtime_error("send failed");
    /* remove Http request instance from connection */
    if (request_)
        delete request_;
    request_ = NULL;
    /* remove Http response instance from connection */
    if (response_)
        delete response_;
    response_ = NULL;
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

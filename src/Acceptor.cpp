/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:25:14 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 16:44:10 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Acceptor.hpp"
#include "ASocket.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

Acceptor::Acceptor() : ASocket() {}

Acceptor::Acceptor(int listenerFd) : ASocket() {
    /* make a new socket for the client */
    socklen_t len = sizeof(addr_);
    fd_ = accept(listenerFd, (struct sockaddr *)&addr_, &len);
    if (fd_ == -1)
        throw std::runtime_error("accept failed");
    if (fcntl(fd_, F_SETFL, O_NONBLOCK))
        throw std::runtime_error("Failed to set socket to non-blocking");
    std::cout << "Accepted connection from " << addr_.sin_port << std::endl;
}

Acceptor::~Acceptor() {
    close(fd_);
}

int Acceptor::getFd() const {
    return fd_;
}

void Acceptor::read() {
    char buff[1024];
    /* read from the client */
    ssize_t rlen = recv(fd_, buff, sizeof(buff) - 1, 0);
    if (rlen == -1)
        throw std::runtime_error("recv failed");
    else if (rlen == 0)
        throw std::runtime_error("client disconnected");
    buff[rlen] = '\0';
    rbuff_ += buff;
}

void Acceptor::write() {
    /* write to the client */
    ssize_t wlen = send(fd_, wbuff_.c_str(), wbuff_.size(), 0);
    if (wlen == -1)
        throw std::runtime_error("send failed");
    /* remove the data that was sent */
    wbuff_.erase(0, wlen);
}

std::string Acceptor::getRbuff() const {
    return rbuff_;
}

std::string Acceptor::getWbuff() const {
    return wbuff_;
}

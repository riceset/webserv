/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 10:40:57 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/16 15:06:41 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"
#include "ASocket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

Listener::Listener() : ASocket() {
    /* /1* initialize listener socket *1/ */
    /* fd_ = socket(AF_INET, SOCK_STREAM, 0); */
    /* if (fd_ == -1) */
    /*     throw std::runtime_error("Failed to create socket"); */
    /* /1* initialize sockaddr struct *1/ */
    /* addr_.sin_family = AF_INET; */
    /* addr_.sin_addr.s_addr = INADDR_ANY; */
    /* addr_.sin_port = htons(8080); */
    /* /1* bind socket to address *1/ */
    /* if (bind(fd_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1) */
    /*     throw std::runtime_error("Failed to bind socket"); */
    /* /1* listen for connections *1/ */
    /* if (listen(fd_, SOMAXCONN) == -1) */
    /*     throw std::runtime_error("Failed to listen for connections"); */
    /* if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1) */
    /*     throw std::runtime_error("Failed to set socket to non-blocking"); */
    /* std::cout << "Listening on port 8080" << std::endl; */
}

Listener::Listener(int port) : ASocket() {
    /* initialize listener socket */
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1)
        throw std::runtime_error("Failed to create socket");
    /* initialize sockaddr struct */
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port);
    /* bind socket to address */
    if (bind(fd_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1)
        throw std::runtime_error("Failed to bind socket");
    /* listen for connections */
    if (listen(fd_, SOMAXCONN) == -1)
        throw std::runtime_error("Failed to listen for connections");
    if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Failed to set socket to non-blocking");
    std::cout << "Listening on port " << port << std::endl;
}

Listener::~Listener() {
    /* close(fd_); */
}

int Listener::getFd() const {
    return fd_;
}

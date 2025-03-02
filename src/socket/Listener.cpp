/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 10:40:57 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/25 17:43:44 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

Listener::Listener() : ASocket() {}

/* Listener constructor */
Listener::Listener(int port) : ASocket() {
	/* initialize listener socket */
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_ == -1)
		throw std::runtime_error("Failed to create socket");
	/* initialize sockaddr struct */
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_.sin_port = htons(port);
	/* bind socket to address */
	if(bind(fd_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1)
		throw std::runtime_error("Failed to bind socket");
	/* listen for connections */
	if(listen(fd_, SOMAXCONN) == -1)
		throw std::runtime_error("Failed to listen for connections");
	if(fcntl(fd_, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket to non-blocking");
	std::cout << "Listening on port " << port << std::endl;
}

Listener::~Listener() {
	/* close(fd_); */
}

int Listener::getFd() const {
	return fd_;
}

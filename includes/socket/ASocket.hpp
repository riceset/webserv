/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 19:27:15 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/25 17:44:59 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASOCKET_HPP
#define ASOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>

/* this is socket addres structure */
/* struct sockaddr_in { */
/*         __uint8_t       sin_len; */
/*         sa_family_t     sin_family; */
/*         in_port_t       sin_port; */
/*         struct  in_addr sin_addr; */
/*         char            sin_zero[8]; */
/* }; */

/* typedef uint32_t in_addr_t; */
/* typedef unsigned short int sa_family_t; */
/* typedef uint16_t in_port_t; */

class ASocket
{
protected:
	int fd_;
	struct sockaddr_in addr_;

public:
	ASocket();
	virtual ~ASocket() = 0;
	virtual int getFd() const = 0;
};

#endif

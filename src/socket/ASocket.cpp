/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 00:33:46 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/25 17:44:56 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocket.hpp"

ASocket::ASocket()
{
	fd_ = 0;
	std::memset(&addr_, 0, sizeof(addr_));
}

ASocket::~ASocket() {}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 00:33:46 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 13:49:12 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocket.hpp"
#include <cstring>

ASocket::ASocket() {
    fd_ = 0;
    std::memset(&addr_, 0, sizeof(addr_));
}

ASocket::~ASocket() {}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWrapper.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 22:49:41 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/26 23:05:54 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionWrapper.hpp"
#include "Connection.hpp"
#include <algorithm>

ConnectionWrapper::ConnectionWrapper() {}

ConnectionWrapper::~ConnectionWrapper() {
    int i = 0;
    while (connections_[i]) {
        delete connections_[i];
        i++;
    }
}

Connection *ConnectionWrapper::getConnection(int fd) const {
    for (unsigned int i = 0; i < connections_.size(); i++) {
        if (connections_[i]->getFd() == fd)
            return connections_[i];
    }
    return NULL;
}

void ConnectionWrapper::removeConnection(int fd) {
    for (unsigned int i = 0; i < connections_.size(); i++) {
        if (connections_[i]->getFd() == fd) {
            delete connections_[i];
            connections_.erase(connections_.begin() + i);
            return;
        }
    }
}

void ConnectionWrapper::addConnection(Connection *conn) {
    connections_.push_back(conn);
}

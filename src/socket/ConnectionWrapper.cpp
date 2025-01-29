/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWrapper.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 22:49:41 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/01/29 14:33:25 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionWrapper.hpp"

#include <algorithm>

#include "Connection.hpp"

ConnectionWrapper::ConnectionWrapper() {}

ConnectionWrapper::~ConnectionWrapper()
{
	int i = 0;
	while(connections_[i])
	{
		delete connections_[i];
		i++;
	}
}

/* Get the connection with the given file descriptor */
Connection *ConnectionWrapper::getConnection(int fd) const
{
	for(unsigned int i = 0; i < connections_.size(); i++)
	{
		if(connections_[i]->getFd() == fd)
			return connections_[i];
	}
	return NULL;
}

/* Remove the connection with the given file descriptor */
void ConnectionWrapper::removeConnection(int fd)
{
	for(unsigned int i = 0; i < connections_.size(); i++)
	{
		if(connections_[i]->getFd() == fd)
		{
			delete connections_[i];
			connections_.erase(connections_.begin() + i);
			return;
		}
	}
}

/* Add a connection to the connection list */
void ConnectionWrapper::addConnection(Connection *conn)
{
	connections_.push_back(conn);
}

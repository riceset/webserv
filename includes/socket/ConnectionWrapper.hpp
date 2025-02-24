/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWrapper.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 22:49:57 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 14:52:20 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONWRAPPER_HPP
#define CONNECTIONWRAPPER_HPP

#include <algorithm>
#include <iostream>
#include <vector>

#include "Connection.hpp"

class ConnectionWrapper
{
private:
	std::vector<Connection *> connections_;

public:
	ConnectionWrapper();
	~ConnectionWrapper();

	// getter
	std::vector<Connection *> getConnections() const;
	Connection *getConnection(int fd) const;

	// setter
	void removeConnection(int fd);
	void addConnection(Connection *conn);
};

#endif

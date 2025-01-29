/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWrapper.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 22:49:57 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/26 23:03:35 by rmatsuba         ###   ########.fr       */
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
	Connection *getConnection(int fd) const;
	void removeConnection(int fd);
	void addConnection(Connection *conn);
};

#endif

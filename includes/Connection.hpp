/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:18:35 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/19 22:52:03 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCELPTOR_HPP
#define ACCELPTOR_HPP

#include <string>

#include "ASocket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Connection : public ASocket
{
private:
	Connection();
	std::string rbuff_;
	std::string wbuff_;
	HttpRequest *request_;
	HttpResponse *response_;

public:
	Connection(int clinentFd);
	~Connection();
	int getFd() const;
	void readSocket();
	void writeSocket();
	std::string getRbuff() const;
	std::string getWbuff() const;
	HttpRequest *getRequest() const;
};

#endif

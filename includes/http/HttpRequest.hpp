/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 18:19:21 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 03:00:45 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <unistd.h>
#include <sys/stat.h>

#include "AHttp.hpp"
#include "MainConf.hpp"

// enum
enum METHOD
{
	GET,
	POST,
	DELETE,
	PUT,
	HEAD,
	OPTIONS,
	TRACE,
	CONNECT,
};

class HttpRequest : public AHttp
{
private:
	HttpRequest();

	std::string location_path_;

	// parse
	std::vector<std::string> parseRequestStartLine(std::string request);
	std::map<std::string, std::string> parseRequestHeader(std::string request);
	std::string parseRequestBody(std::string request);

public:
	// constructor
	HttpRequest(std::string request);
	~HttpRequest();

	// getter
	std::vector<std::string> getStartLine() const;
	std::map<std::string, std::string> getHeader() const;
	std::string getBody() const;
	std::string getLocationPath() const;
	METHOD getMethod() const;

	// checker
	bool hasError(MainConf &mainConf);
};

#endif

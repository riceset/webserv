/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 18:19:08 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/01/29 19:57:26 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

/* Constructor */
HttpRequest::HttpRequest(std::string request)
{
	start_line_.resize(3);
	start_line_ = parseRequestStartLine(request);
	headers_ = parseRequestHeader(request);
	body_ = parseRequestBody(request);
}

std::vector<std::string> HttpRequest::getStartLine() const
{
	return start_line_;
}

std::map<std::string, std::string> HttpRequest::getHeader() const
{
	return headers_;
}

std::string HttpRequest::getBody() const
{
	return body_;
}

/* Parse StartLine of the request */
std::vector<std::string> HttpRequest::parseRequestStartLine(std::string request)
{
	std::string first_line = request.substr(0, request.find("\r\n"));
	std::istringstream ss(first_line);
	std::vector<std::string> start_line;
	while(ss)
	{
		std::string token;
		std::getline(ss, token, ' ');
		if(!token.empty())
			start_line.push_back(token);
	}
	return start_line;
}

/* Parse Header of the request */
std::map<std::string, std::string> HttpRequest::parseRequestHeader(
	std::string request)
{
	std::map<std::string, std::string> header;
	size_t start = request.find("\r\n") + 2;
	size_t end = request.find("\r\n\r\n");
	if(start > end)
		throw std::runtime_error("Header not found");
	std::string header_str = request.substr(start, end - start);
	std::istringstream ss(header_str);
	while(true)
	{
		std::string line;
		std::getline(ss, line, '\n');
		std::string key = line.substr(0, line.find(":"));
		std::string value = line.substr(line.find(":") + 1, line.find("\r"));
		header[key] = value;
		if(ss.eof())
			break;
	}
	return header;
}

/* Parse Body of the request */
std::string HttpRequest::parseRequestBody(std::string request)
{
	std::string body = request.substr(request.find("\r\n\r\n") + 4);
	return body;
}

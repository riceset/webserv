/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 18:19:08 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 03:04:34 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// ==================================== constructor and destructor ====================================

HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(std::string request)
{
	start_line_.resize(3);
	start_line_ = parseRequestStartLine(request);
	headers_ = parseRequestHeader(request);
	body_ = parseRequestBody(request);
}

// ==================================== getter ====================================

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

std::string HttpRequest::getLocationPath() const
{
	return location_path_;
}

METHOD HttpRequest::getMethod() const
{
	std::string method = start_line_[0];
	if(method == "GET")
		return GET;
	else if(method == "POST")
		return POST;
	else if(method == "DELETE")
		return DELETE;
	else if(method == "PUT")
		return PUT;
	else if(method == "HEAD")
		return HEAD;
}
		

// ==================================== setter ====================================

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
		std::getline(ss, line);
		int count = 0;
		for (long unsigned int i = 0; i < line.size(); i++) {
			if (line[i] == '\r' || line[i] == '\n') {
				count++;
			}
			i++;
		}
		line = line.substr(0, line.size() - count);
		std::string key = line.substr(0, line.find(":"));
		std::string value = line.substr(line.find(":") + 2);
		/* std::cout << "key: " << key << std::endl; */
		/* std::cout << "value: " << value << std::endl; */
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

// ==================================== check ==============================================

std::string joinPath(std::string request_path, conf_value_t conf_value) {
	std::string location_path;
	struct stat st;

	bool is_directory = false;
	if (request_path[request_path.size() - 1] == '/')
		is_directory = true;
	if (is_directory) {
		for (size_t i = 0; i < conf_value._index.size(); i++) {
			std::string index_path = conf_value._index[i];
			if (index_path[0] == '/')
				index_path = index_path.substr(1);
			location_path = "." + conf_value._root + request_path + index_path;
			if (stat(location_path.c_str(), &st) == 0)
				return location_path;
			throw std::runtime_error("file not found");
		}
	} else {
		location_path = "." + conf_value._root + request_path;
		std::cout << "location_path: " << location_path << std::endl;
		if (stat(location_path.c_str(), &st) == 0)
			return location_path;
		throw std::runtime_error("file not found");
	}
	return "";
}

bool HttpRequest::hasError(MainConf &mainConf)
{
	std::string request_path = start_line_[1];

	std::string server_and_port = headers_["Host"];
	int pos = server_and_port.find(":");
	std::string server_name = server_and_port.substr(0, pos);
	std::string port = server_and_port.substr(pos + 1);
	std::string location_path;

	
	try {
		location_path = joinPath(request_path, mainConf.get_conf_value(port, server_name, request_path));
		location_path_ = location_path;
	} catch (std::runtime_error &e) {
		// todo 404の処理
		return true;
	}

	return false;
}

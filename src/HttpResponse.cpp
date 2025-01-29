/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 17:52:45 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/01/29 20:01:47 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <string>
#include <vector>
#include "HttpRequest.hpp"

HttpResponse::HttpResponse()
{
	start_line_.resize(3);
	status_code_[200] = "OK";
	status_code_[404] = "Not Found";
	status_code_[501] = "Not Implemented";
}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(HttpRequest *request)
{
	start_line_ = setResponseStartLine(request->getStartLine());
	headers_ = setResponseHeader(request->getHeader());
	body_ = setResponseBody(request->getBody());
}

std::vector<std::string> HttpResponse::setResponseStartLine(
	std::vector<std::string> requestStartLine)
{
	std::vector<std::string> start_line;
	bool is_valid_method = isValidMethod(requestStartLine[0]);
	bool is_valid_path = isValidPath(requestStartLine[1]);
	if(isValidVersion(requestStartLine[2]))
		start_line.push_back(requestStartLine[2]);
	else
		throw std::runtime_error("Invalid HTTP version");
	if(is_valid_method == false)
	{
		setStatusCode(501, status_code_[501], start_line);
		return start_line;
	}
	else if(is_valid_path == false)
	{
		setStatusCode(404, status_code_[404], start_line);
		return start_line;
	}
	setStatusCode(200, status_code_[200], start_line);
	return start_line;
}

void HttpResponse::setStatusCode(int code,
								 std::string message,
								 std::vector<std::string> &start_line)
{
	std::istringstream ss;
	ss << code;
	start_line_.push_back(ss.str());
	start_line_.push_back(message);
}

bool isValidVersion(std::string version)
{
	if(version != "HTTP/1.1")
		return false;
	return true;
}

bool isValidMethod(std::string method)
{
	if(method != "GET" || method != "POST" || method != "DELETE")
		return false;
	return true;
}

bool isValidPath(std::string resourse_path)
{
	std::string root = "./www";
	if(resourse_path == "/")
		resourse_path = "/index.html";
	std::string path = root + resourse_path;
	struct stat st;
	if(stat(path.c_str(), &st) == 0)
		return true;
	return false;
}

std::vector<std::string> HttpResponse::getStartLine() const
{
	return start_line_;
}

std::map<std::string, std::string> HttpResponse::getHeader() const
{
	return headers_;
}

std::string HttpResponse::getBody() const
{
	return body_;
}

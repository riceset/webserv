/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 17:52:45 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/10 19:06:43 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "HttpRequest.hpp"
#include "MainConf.hpp"

std::map<int, std::string> HttpResponse::status_code_;

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

/* Constructor */
HttpResponse::HttpResponse(HttpRequest *request, MainConf *mainConf)
{
	start_line_.resize(3);
	initializeStatusCodes();
	std::string server_and_port = request->getHeader()["Host"];
	int pos = server_and_port.find(":");
	std::string server_name = server_and_port.substr(0, pos);
	std::string port = server_and_port.substr(pos + 1);
	std::string requestPath = request->getStartLine()[1];
	conf_value_t conf_value = mainConf->get_conf_value(port, server_name, requestPath);
	processResponseStartLine(request->getStartLine(), conf_value);
	processResponseBody(request->getStartLine(), conf_value);
	processResponseHeader(request->getHeader(), conf_value);
}

/* Make map data of status codes and messages */
void HttpResponse::initializeStatusCodes()
{
	status_code_[200] = "OK";
	status_code_[400] = "Bad Request";
	status_code_[404] = "Not Found";
	status_code_[405] = "Method Not Allowed";
	status_code_[414] = "URI Too Long";
	status_code_[413] = "Content Too Long";
	status_code_[501] = "Not Implemented";
	status_code_[505] = "HTTP Version Not Supported";
}

/* Set the start line of the response */
void HttpResponse::processResponseStartLine(std::vector<std::string> requestStartLine, conf_value_t conf_value)
{
	/* Check the validity of the HTTP method */
	if (isValidHttpVersion(requestStartLine[0]) == false)
	{
		setResponseStartLine(505);
		return;
	} else if (isValidHttpMethod(requestStartLine[0], conf_value._limit_except) == false)
	{
		setResponseStartLine(405);
		return;
	} else if (isValidPath(requestStartLine[1], conf_value) == false)
	{
		setResponseStartLine(404);
		return;
	}
	setResponseStartLine(200);
	return ;
}

/* check http version */
bool HttpResponse::isValidHttpVersion(std::string version)
{
	if (version != "HTTP/1.1")
		return false;
	return true;
}

/* check http method */
bool HttpResponse::isValidHttpMethod(std::string method, std::vector<std::string> limit_except) {
	if (std::find(limit_except.begin(), limit_except.end(), method) == limit_except.end()) 
		return false;
	return true;
}

bool HttpResponse::isValidPath(std::string request_path, conf_value_t conf_value) {
	/* in this process, check only the existence of the requested path */
	/* where error_page exist or not is not checked */ 
	/* make requested path that is based wevserv root */
	if (getLocationPath(request_path, conf_value) == "")
		return false;
	return true;
}

std::string HttpResponse::getLocationPath(std::string request_path, conf_value_t conf_value) {
	std::string location_path;
	struct stat st;
	/* if request_path is directory, check the existence of index file */
	if (std::find(request_path.begin(), request_path.end(), '/') == request_path.end()) {
		for (size_t i = 0; i < conf_value._index.size(); i++) {
			location_path = "." + conf_value._root + request_path + conf_value._index[i];
			if (stat(location_path.c_str(), &st) == 0)
				return location_path;
		}
	} else {
		location_path = "." + conf_value._root + request_path;
		if (stat(location_path.c_str(), &st) == 0)
			return location_path;
	}
	return "";
}

/* Set the start line of the response */
void HttpResponse::setResponseStartLine(int status_code) {
	std::ostringstream oss;
	oss << status_code;
	start_line_[0] = "HTTP/1.1";
	start_line_[1] = oss.str();
	start_line_[2] = status_code_[status_code];
}

/* Set the header of the response */
void HttpResponse::processResponseHeader(std::map<std::string, std::string> requestHeader, conf_value_t conf_value) {
	headers_["Date"] = setDate();
	headers_["Server"] = conf_value._server_name;
	headers_["Content-Type"] = requestHeader["Accept"];
	headers_["Content-Language"] = requestHeader["Accept-Language"];
	headers_["Keep-Alive"] = "timeout=5, max=100";
	headers_["Connection"] = requestHeader["Connection"];
	std::size_t body_length = body_.size();
	std::ostringstream ss;
	ss << body_length;
	headers_["Content-Length"] = ss.str();
}

/* make date data for response header */
std::string HttpResponse::setDate()
{
	std::time_t now = std::time(NULL);
	std::string date = std::ctime(&now);
	std::vector<std::string> date_vec;
	std::stringstream ss(date);
	char space = ' ';
	while(getline(ss, date, space))
	{
		if(!date.empty() && date != "\n")
			date_vec.push_back(date);
	}
	date_vec[4] = date_vec[4].substr(0, date_vec[4].size() - 1);
	std::string date_str = date_vec[0] + ", " + date_vec[2] + " " +
						   date_vec[1] + " " + date_vec[4] + " " + date_vec[3] +
						   " GMT";
	return date_str;
}

/* Set the body of the response */
void HttpResponse::processResponseBody(std::vector<std::string> requestStartLine, conf_value_t conf_value) {
	std::string location_path = getLocationPath(requestStartLine[1], conf_value);
	if (location_path == "") {
		std::string error_page = "." + conf_value._error_page.back();
		std::ifstream ifs(error_page.c_str());
		if (!ifs) {
			body_ = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>";
		} else {
			std::string line;
			while (getline(ifs, line)) {
				body_ += line;
			}
		}
	} else {
		std::ifstream ifs(location_path.c_str());
		std::string line;
		while (getline(ifs, line)) {
			body_ += line;
		}
	}
	return ;
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

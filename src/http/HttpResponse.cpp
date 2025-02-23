/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 17:52:45 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 02:08:07 by atsu             ###   ########.fr       */
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
#include "CGI.hpp"

std::map<int, std::string> HttpResponse::status_codes_; // ? ここで用いるのは適切か？　header or static に移動させるべきかもしれない

/* Make map data of status codes and messages */
void HttpResponse::initializeStatusCodes()
{
	status_codes_[200] = "OK";
	status_codes_[400] = "Bad Request";
	status_codes_[404] = "Not Found";
	status_codes_[405] = "Method Not Allowed";
	status_codes_[414] = "URI Too Long";
	status_codes_[413] = "Content Too Long";
	status_codes_[501] = "Not Implemented";
	status_codes_[505] = "HTTP Version Not Supported";
}

// ==================================== constructor and destructor ====================================

HttpResponse::HttpResponse()
{
	start_line_.resize(3);
	initializeStatusCodes();
}

HttpResponse::~HttpResponse() {}

// ==================================== setter ====================================

/* Set the start line of the response */
/* ここの処理は、この先変更する必要がありそう。if分岐ではなくて、再帰的にエラーを処理していけば、最も適切なエラーを書い得せるようにできそう。 */
void HttpResponse::processResponseStartLine(std::vector<std::string> requestStartLine, conf_value_t conf_value)
{
	/* Check the validity of the HTTP method */
	if (isValidHttpVersion(requestStartLine[2]) == false)
	{
		status_code_ = 505;
		setResponseStartLine(505);
		return;
	}
	else if (isValidHttpMethod(requestStartLine[0], conf_value._limit_except) == false)
	{
		status_code_ = 405;
		setResponseStartLine(405);
		return;
	}
	else if (isValidPath(requestStartLine[1], conf_value) == false)
	{
		status_code_ = 404;
		setResponseStartLine(404);
		return;
	}
	status_code_ = 200;
	setResponseStartLine(200);
	return ;
}

/* Set the start line of the response */
void HttpResponse::setResponseStartLine(int status_code) {
	std::ostringstream oss;
	oss << status_code;
	start_line_[0] = "HTTP/1.1";
	start_line_[1] = oss.str();
	start_line_[2] = status_codes_[status_code];
}

std::string HttpResponse::checkContentType(std::string request_path, conf_value_t conf_value) {
	std::string location_path = getLocationPath(request_path, conf_value);
	std::string content_type;

	std::string extension = location_path.substr(location_path.find_last_of(".") + 1);
	if (extension == "html")
		content_type = "text/html";
	else if (extension == "json")
		content_type = "application/json";
	return content_type;
}

/* Set the header of the response */
void HttpResponse::processResponseHeader(std::map<std::string, std::string> requestHeader, conf_value_t conf_value, std::string request_path) {
	headers_["Date"] = setDate();
	headers_["Server"] = conf_value._server_name;
	headers_["Content-Type"] = checkContentType(request_path, conf_value);
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
	int path_size = location_path.size();
	/* std::cout << "location_path: " << location_path << std::endl; */
	if (location_path == "") {
		std::string error_page = "." + conf_value._root + conf_value._error_page.back();
		std::cout << "error_page: " << error_page << std::endl;
		std::ifstream ifs(error_page.c_str());
		if (!ifs) {
			body_ = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>";
		} else {
			std::string line;
			while (getline(ifs, line)) {
				body_ += line;
			}
		}
	}
	else if (path_size > 4 && location_path.substr(path_size - 4, 4) == ".php") { // cgiに渡すべきかどうかの検証 抜根的な改革が必要
		CGI cgi(location_path);
		body_ = cgi.getFd();
	} else {
		// ? この処理をfdの中に入れるべきだと考える
		std::ifstream ifs(location_path.c_str());
		std::string line;
		while (getline(ifs, line)) {
			body_ += line;
		}
	}
	return ;
}

// ==================================== getter ====================================

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

int HttpResponse::getStatusCode() const
{
	return status_code_;
}

// ==================================== checker ====================================

bool HttpResponse::isValidHttpVersion(std::string version)
{
	if (version != "HTTP/1.1")
		return false;
	return true;
}

/* this process needs to add the process of distinguishing http method */ 
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

// ==================================== utils ====================================

std::string HttpResponse::getLocationPath(std::string request_path, conf_value_t conf_value) {
	std::string location_path;
	struct stat st;
	/* if request_path is directory, check the existence of index file */
	/* std::cout << "request_path: " << request_path << std::endl; */
	bool is_directory = false;
	if (request_path[request_path.size() - 1] == '/')
		is_directory = true;
	if (is_directory) {
		for (size_t i = 0; i < conf_value._index.size(); i++) {
			/* std::cout << "index: " << conf_value._index[i] << std::endl; */
			std::string index_path = conf_value._index[i];
			if (index_path[0] == '/')
				index_path = index_path.substr(1);
			location_path = "." + conf_value._root + request_path + index_path;
			/* std::cout << "location_path: " << location_path << std::endl; */
			if (stat(location_path.c_str(), &st) == 0)
				return location_path;
		}
	} else {
		location_path = "." + conf_value._root + request_path;
		std::cout << "location_path: " << location_path << std::endl;
		if (stat(location_path.c_str(), &st) == 0)
			return location_path;
	}
	return "";
}
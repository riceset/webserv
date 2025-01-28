/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 17:52:45 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/27 20:09:38 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <ctime>

std::map<int, std::string> HttpResponse::status_code_;

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(HttpRequest *request) {
    start_line_.resize(3);
    initializeStatusCodes();
    start_line_ = setResponseStartLine(request->getStartLine());
    body_ = setResponseBody(request->getStartLine());
    headers_ = setResponseHeader(request->getHeader());
}

void HttpResponse::initializeStatusCodes() {
    status_code_[200] = "OK";
    status_code_[400] = "Bad Request";
    status_code_[404] = "Not Found";
    status_code_[414] = "URI Too Long";
    status_code_[413] = "Content Too Long";
    status_code_[501] = "Not Implemented";
}

int HttpResponse::checkStatusCode(std::vector<std::string> requestStartLine) {
    if (!isValidMethod(requestStartLine[0]))
        return 501;
    if (!isValidPath(requestStartLine[1]))
        return 404;
    if (!isValidVersion(requestStartLine[2]))
        return 501;
    return 200;
}

std::vector<std::string> HttpResponse::setResponseStartLine(std::vector<std::string> requestStartLine) {
    std::vector<std::string> start_line;
    bool is_valid_method = isValidMethod(requestStartLine[0]);
    bool is_valid_path = isValidPath(requestStartLine[1]);
    if (isValidVersion(requestStartLine[2]))
        start_line.push_back(requestStartLine[2]);
    else
        throw std::runtime_error("Invalid HTTP version");
    if (is_valid_method == false) {
        setStatusCode(501, status_code_[501], start_line);
        return start_line;
    } else if (is_valid_path == false) {
        setStatusCode(404, status_code_[404], start_line);
    }
    return start_line;
}

bool HttpResponse::isValidMethod(std::string method) {
    if (method == "GET" || method == "POST" || method == "DELETE")
        return true;
    return false;
}

bool HttpResponse::isValidPath(std::string resourse_path) {
    std::string root = "./www";
    if (resourse_path == "/")
        resourse_path = "/index.html";
    std::string path = root + resourse_path;
    struct stat st;
    if (stat(path.c_str(), &st) == 0)
        return true;
    return false;
}

std::string HttpResponse::setDate() {
    std::time_t now = std::time(NULL);
    std::string date = std::ctime(&now);
    std::vector<std::string> date_vec;
    std::stringstream ss(date);
    char space = ' ';
    while (getline(ss, date, space)) {
        if (!date.empty() && date != "\n")
            date_vec.push_back(date);
    }
    date_vec[4] = date_vec[4].substr(0, date_vec[4].size() - 1);
    std::string date_str = date_vec[0] + ", " + date_vec[2] + " " + date_vec[1] + " " + date_vec[4] + " " + date_vec[3] + " GMT";
    return date_str;
}

std::map<std::string, std::string> HttpResponse::setResponseHeader(std::map<std::string, std::string> requestHeader) {
    (void)requestHeader;
    std::map<std::string, std::string> headers;
    headers["Date"] = setDate();
    headers["Server"] = "localhost";
    headers["Content-Type"] = "text/html";
    headers["Connection"] = "keep-alive";
    headers["Keep-Alive"] = "timeout=5, max=1000";
    std::size_t body_length = body_.size();
    std::ostringstream ss;
    ss << body_length;
    headers["Content-Length"] = ss.str();
    return headers;
}

std::string HttpResponse::setResponseBody(std::vector<std::string> requestStartLine) {
    std::string root = "./www";
    std::string requestedpath = requestStartLine[1];
    if (requestedpath == "/")
        requestedpath = "/index.html";
    std::string filepath = root + requestedpath;
    std::string body;
    std::ifstream ifs(filepath.c_str());
    if (ifs.fail())
        throw std::runtime_error("Failed to open file");
    std::string line;
    while (std::getline(ifs, line))
        body += line;
    ifs.close();
    return body;
}

void HttpResponse::setStatusCode(int code, std::string message, std::vector<std::string> &start_line) {
    std::ostringstream ss;
    ss << code;
    start_line.push_back(ss.str());
    start_line.push_back(message);
}

bool HttpResponse::isValidVersion(std::string version) {
    if (version != "HTTP/1.1")
        return false;
    return true;
}


std::vector<std::string> HttpResponse::getStartLine() const {
    return start_line_;
}

std::map<std::string, std::string> HttpResponse::getHeader() const {
    return headers_;
}

std::string HttpResponse::getBody() const {
    return body_;
}


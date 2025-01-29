/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:40:12 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/01/29 19:55:40 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "AHttp.hpp"
#include "HttpRequest.hpp"

class HttpResponse : public AHttp
{
private:
	std::vector<std::string> setResponseStartLine(
		std::vector<std::string> requestStartLine);
	std::map<std::string, std::string> setResponseHeader(
		std::map<std::string, std::string> requestHeader);
	std::string setResponseBody(std::vector<std::string> requestStartLine);
	HttpResponse();
	static std::map<int, std::string> status_code_;

public:
	HttpResponse(HttpRequest *request);
	~HttpResponse();
	std::vector<std::string> getStartLine() const;
	std::map<std::string, std::string> getHeader() const;
	std::string getBody() const;
	bool isValidVersion(std::string version);
	bool isValidMethod(std::string method);
	bool isValidPath(std::string resourse_path);
	void setStatusCode(int, std::string, std::vector<std::string> &start_line);
	static void initializeStatusCodes();
	std::string setDate();
	int checkStatusCode(std::vector<std::string> requestStartLine);
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:40:12 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/13 13:49:35 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "AHttp.hpp"
#include "HttpRequest.hpp"
#include "MainConf.hpp"

class HttpResponse : public AHttp
{
	private:
		void processResponseStartLine(std::vector<std::string> requestStartLine, conf_value_t conf_value);
		void processResponseHeader(std::map<std::string, std::string> requestHeader, conf_value_t conf_value, std::string request_path);
		void processResponseBody(std::vector<std::string> requestStartLine, conf_value_t conf_value);
		HttpResponse();
		static std::map<int, std::string> status_code_;
	public:
		bool isValidHttpVersion(std::string version);
		bool isValidHttpMethod(std::string method, std::vector<std::string> limit_except);
		bool isValidPath(std::string request_path, conf_value_t conf_value);
		void setResponseStartLine(int status_code);
		std::string getLocationPath(std::string request_path, conf_value_t conf_value);
		std::string setDate();
		std::vector<std::string> getStartLine() const;
		std::map<std::string, std::string> getHeader() const;
		std::string getBody() const;
		HttpResponse(HttpRequest *request, MainConf *mainConf);
		~HttpResponse();
		static void initializeStatusCodes();
		std::string checkContentType(std::string request_path, conf_value_t conf_value);
};
#endif

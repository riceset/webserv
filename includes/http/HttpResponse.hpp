/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:40:12 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/23 02:58:14 by atsu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "AHttp.hpp"
#include "HttpRequest.hpp"
#include "MainConf.hpp"

class HttpResponse : public AHttp
{
	private:
		HttpResponse();
		static std::map<int, std::string> status_code_;

		// setter
		void processResponseStartLine(std::vector<std::string> requestStartLine, conf_value_t conf_value);
		void processResponseHeader(std::map<std::string, std::string> requestHeader, conf_value_t conf_value, std::string request_path);
		void processResponseBody(std::vector<std::string> requestStartLine, conf_value_t conf_value);

	public:
		// constructor
		HttpResponse(HttpRequest *request, MainConf *mainConf);
		~HttpResponse();

		// error check
		bool isValidHttpVersion(std::string version);
		bool isValidHttpMethod(std::string method, std::vector<std::string> limit_except);
		bool isValidPath(std::string request_path, conf_value_t conf_value);
		std::string checkContentType(std::string request_path, conf_value_t conf_value);

		// setter
		static void initializeStatusCodes();
		void setResponseStartLine(int status_code);
		std::string setDate();

		// getter
		std::string getLocationPath(std::string request_path, conf_value_t conf_value);
		std::vector<std::string> getStartLine() const;
		std::map<std::string, std::string> getHeader() const;
		std::string getBody() const;
};

#endif

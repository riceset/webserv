/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atsu <atsu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:40:12 by rmatsuba          #+#    #+#             */
/*   Updated: 2025/02/25 16:29:03 by atsu             ###   ########.fr       */
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
		int status_code_;
		static std::map<int, std::string> status_codes_; // ? Ahttp に移動させるべきかもしれない

	public:
		// constructor
		HttpResponse();
		~HttpResponse();

		// error check
		// bool isValidHttpVersion(std::string version);
		// bool isValidHttpMethod(std::string method, std::vector<std::string> limit_except);
		// bool isValidPath(std::string request_path, conf_value_t conf_value);

		// getter
		std::vector<std::string> getStartLine() const;
		std::map<std::string, std::string> getHeader() const;
		std::string getBody() const;

		// setter
		static void initializeStatusCodes();
		void setStartLine(int status_code);
		void setBody(std::string buff);
		void setHeader(std::map<std::string, std::string> requestHeader, std::string path, std::string server_name);
		// void setBody(std::vector<std::string> requestStartLine, conf_value_t conf_value);
		// void setStartLine(std::vector<std::string> requestStartLine, conf_value_t conf_value);

		std::string buildResponse();

		// util
		std::string getContentType(std::string path);
		std::string getDate();
};

#endif

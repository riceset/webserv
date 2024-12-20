/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 18:19:21 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/19 20:06:09 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "AHttp.hpp"

class HttpRequest : public AHttp {
    private:
        std::vector<std::string> parseRequestStartLine(std::string request);
        std::map<std::string, std::string> parseRequestHeader(std::string request);
        std::string parseRequestBody(std::string request);
        HttpRequest();
    public:
        HttpRequest(std::string request);
        ~HttpRequest();
        std::vector<std::string> getStartLine() const;
        std::map<std::string, std::string> getHeader() const;
        std::string getBody() const;
};


#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:40:12 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/19 20:06:31 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "AHttp.hpp"
#include "HttpRequest.hpp"

class HttpResponse : public AHttp {
    private:
        std::vector<std::string> setResponseStartLine();
        std::map<std::string, std::string> setResponseHeader();
        std::string setResponseBody();
    public:
        HttpResponse();
        ~HttpResponse();
        std::vector<std::string> getStartLine() const;
        std::map<std::string, std::string> getHeader() const;
        std::string getBody() const;
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 17:52:45 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/19 20:34:30 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <string>
#include <vector>
#include <map>

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

std::vector<std::string> HttpResponse::getStartLine() const {
    return start_line_;
}

std::map<std::string, std::string> HttpResponse::getHeader() const {
    return headers_;
}

std::string HttpResponse::getBody() const {
    return body_;
}


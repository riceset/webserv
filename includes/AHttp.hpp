/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttp.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 18:29:07 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/20 19:15:31 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AHTTP_HPP
# define AHTTP_HPP

#include <string>
#include <map>
#include <vector>

class AHttp {
    protected:
        std::vector<std::string> start_line_;
        std::map<std::string, std::string> headers_;
        std::string body_;
    public:
        AHttp();
        virtual ~AHttp() = 0;
        virtual std::vector<std::string> getStartLine() const = 0;
        virtual std::map<std::string, std::string> getHeader() const = 0;
        virtual std::string getBody() const = 0;
        class 
};

#endif

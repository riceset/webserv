/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:18:35 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 16:43:57 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCELPTOR_HPP
# define ACCELPTOR_HPP

#include "ASocket.hpp"
#include <string>

class Acceptor : public ASocket {
    private:
        Acceptor();
        std::string rbuff_;
        std::string wbuff_;
    public:
        Acceptor(int clinentFd);
        ~Acceptor();
        int getFd() const;
        void read();
        void write();
        std::string getRbuff() const;
        std::string getWbuff() const;
};

#endif

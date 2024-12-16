/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:18:35 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/16 00:48:18 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCELPTOR_HPP
# define ACCELPTOR_HPP

#include "ASocket.hpp"
#include <string>

class Connection: public ASocket {
    private:
        Connection();
        std::string rbuff_;
        std::string wbuff_;
    public:
        Connection(int clinentFd);
        ~Connection();
        int getFd() const;
        void read();
        void write();
        std::string getRbuff() const;
        std::string getWbuff() const;
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:18:35 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/26 22:55:04 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCELPTOR_HPP
# define ACCELPTOR_HPP

#include "ASocket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>
#include <ctime>

class Connection: public ASocket {
    private:
        Connection();
        std::string rbuff_;
        std::string wbuff_;
        HttpRequest *request_;
        HttpResponse *response_;
        std::time_t lastActive_;
        static std::time_t timeout_;
    public:
        Connection(int clinentFd);
        ~Connection();
        int getFd() const;
        void readSocket();
        void writeSocket();
        std::string getRbuff() const;
        std::string getWbuff() const;
        HttpRequest *getRequest() const;
        bool isTimedOut();
        void buildResponseString();
};

/* Connection *getConnection(std::vector<Connection *> &connections, int fd); */
std::string vecToString(std::vector<std::string> vec);
std::string mapToString(std::map<std::string, std::string>);
#endif

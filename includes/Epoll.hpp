/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 14:08:59 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 18:36:31 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

#include <sys/epoll.h>
#include <vector>
#include <stdexcept>
#include "EpollEvent.hpp"

class Epoll {
    private:
        int epfd_;
        size_t max_events_;
        std::vector<struct epoll_event*> events_list_;
        Epoll();
    public:
        Epoll(int max_events);
        ~Epoll();
        int getEpfd() const;
        std::vector<struct epoll_event> getEventsList() const;
        void addEvent();
        int epwait();
};

#endif
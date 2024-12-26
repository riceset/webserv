/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 14:08:59 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/22 23:07:42 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include <sys/epoll.h>
#include <vector>
#include <stdexcept>

class EpollWrapper {
    private:
        int epfd_;
        size_t max_events_;
        std::vector<struct epoll_event> events_list_;
        EpollWrapper();
    public:
        EpollWrapper(int max_events);
        ~EpollWrapper();
        int getEpfd() const;
        std::vector<struct epoll_event> getEventsList() const;
        void addEvent(int fd);
        void deleteEvent(int fd);
        int epwait();
        struct epoll_event &operator[](size_t index);
        void setEvent(int fd, uint32_t events);
};

#endif

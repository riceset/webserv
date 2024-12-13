/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollEvent.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 14:47:53 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 16:56:39 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLEVENT_HPP
# define EPOLLEVENT_HPP

#include <sys/epoll.h>

class EpollEvent {
    private:
        struct epoll_event event_;
        EpollEvent();
    public:
        EpollEvent(int epfd, int event_fd);
        ~EpollEvent();
        void setEvent(int event);
        struct epoll_event getEvent() const;
};

#endif

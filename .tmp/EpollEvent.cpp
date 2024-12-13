/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollEvent.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 15:46:59 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 16:56:49 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollEvent.hpp"
#include <stdexcept>
#include <iostream>


EpollEvent::EpollEvent() {}

EpollEvent::EpollEvent(int epfd, int event_fd) {
    event_.events = EPOLLIN;
    event_.data.fd = event_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, event_fd, &event_) == -1)
        throw std::runtime_error("Failed to add event to epoll");
}

EpollEvent::~EpollEvent() {}

void EpollEvent::setEvent(int event) {
    event_.events = event;
}

struct epoll_event EpollEvent::getEvent() const {
    return event_;
}

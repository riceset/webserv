/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 14:29:33 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/16 01:05:53 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include <stdexcept>
#include <unistd.h>

Epoll::Epoll() {}

Epoll::Epoll(int max_events) {
    epfd_ = epoll_create1(0);
    if (epfd_ == -1)
        throw std::runtime_error("Failed to create epoll instance"); 
    events_list_.resize(max_events);
    max_events_ = max_events;
}

Epoll::~Epoll() {
    close(epfd_);
}

int Epoll::getEpfd() const {
    return epfd_;
}

std::vector<struct epoll_event> Epoll::getEventsList() const {
    return events_list_;
}

void Epoll::addEvent(int fd) {
    struct epoll_event new_event;
    new_event.events = EPOLLIN;
    new_event.data.fd = fd;
    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &new_event) == -1)
        throw std::runtime_error("Failed to add event to epoll instance");
}

int Epoll::epwait() {
    int nfds = epoll_wait(epfd_, events_list_.data(), events_list_.size(), -1);
    if (nfds == -1)
        throw std::runtime_error("epoll_wait failed");
    return nfds;
}



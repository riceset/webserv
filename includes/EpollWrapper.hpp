/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollWrapper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 14:08:59 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/20 00:26:11 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include <sys/epoll.h>

#include <stdexcept>
#include <vector>

class EpollWrapper
{
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
};

#endif

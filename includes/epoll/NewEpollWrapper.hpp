#pragma once

#include <sys/epoll.h>
#include <unistd.h>

#include <stdexcept>
#include <vector>

#include "ApollWrapper.hpp"

class EpollWrapper : public ApollWrapper
{
private:
	int epfd_;
	std::vector<struct epoll_event> _events;
	std::vector<struct epoll_event> _detected_events;
	int _detected_event_size;
	EpollWrapper();

public:
	// constructor
	EpollWrapper(int max_events, std::vector<Listener> listeners);
	~EpollWrapper();

	// setter
	void addListener(int port);

	void addEvent(int fd, uint32_t events);
	void modifyEvent(int fd, uint32_t events);
	void removeEvent(int fd);

	// getter
	int getEpfd() const;
	int getEventSize() const;

	// methods
	void wait();

	bool isListener(int index);
	bool isPollinEvent(int index);
	bool isPolloutEvent(int index);
	bool isTimeOut(int index);

	void accept(int index);
	void read(int index);
	void write(int index);
	void closeSocket(int index);
};

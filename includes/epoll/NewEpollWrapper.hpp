#pragma once

#include <sys/epoll.h>

#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <stdexcept>

#include "ApollWrapper.hpp"


class EpollWrapper : public ApollWrapper
{
private:
	int epfd_;
	std::vector<struct epoll_event> _events;
	EpollWrapper();

public:
	// constructor
	EpollWrapper(int max_events);
	~EpollWrapper();

	// setter
	void addListener(int port);

	void addEvent(int fd);
	void modifyEvent(int fd, uint32_t events);
	void removeEvent(int fd);

	// getter
	int getEpfd() const;
	int getEventSize() const;

	// methods
	void wait();

	bool is_listener(int index);
	bool is_pollin_event(int index);
	bool is_pollout_event(int index);

	void accept(int index);
	void read(int index);
	void write(int index);
};

#endif

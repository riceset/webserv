#pragma once

#include <vector>
#include "Listener.hpp"
#include "Connection.hpp"
#include "ConnectionWrapper.hpp"

class ApollWrapper
{
protected:
	std::vector<Listener> _listeners;
	ConnectionWrapper _connections;
	// std::vector<event> events; // struct epoll_event or struct pollfd

public:
	// constructor
	ApollWrapper();
	~ApollWrapper();

	// setter
	virtual void addListener(int port) = 0;

	// void addEvent(int fd, uint32_t events); // struct epoll_event or struct pollfd
	// void modifyEvent(int fd, uint32_t events); // struct epoll_event or struct pollfd

	// getter
	virtual int getEventNumber() const = 0;

	// methods
	virtual void wait() = 0;
	virtual bool is_listener(int index) = 0;
	virtual bool is_pollin_event(int index) = 0;
	virtual bool is_pollout_event(int index) = 0;

	virtual void accept(int index) = 0;
	virtual void read(int index) = 0;
	virtual void write(int index) = 0;
};

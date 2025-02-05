#pragma once

#include <vector>

#include "Connection.hpp"
#include "ConnectionWrapper.hpp"
#include "Listener.hpp"

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

	// void addEvent(int fd, uint32_t events); // struct epoll_event or struct
	// pollfd void modifyEvent(int fd, uint32_t events); // struct epoll_event
	// or struct pollfd

	// getter
	virtual int getEventSize() const = 0;

	// methods
	virtual void wait() = 0;
	virtual bool isListener(int index) = 0;
	virtual bool isPollinEvent(int index) = 0;
	virtual bool isPolloutEvent(int index) = 0;

	virtual void accept(int index) = 0;
	virtual void read(int index) = 0;
	virtual void write(int index) = 0;
};

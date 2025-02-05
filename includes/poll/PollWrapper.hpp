#pragma once

#include <poll.h>

#include <fstream>
#include <vector>

#include "ApollWrapper.hpp"

class PollWrapper : public ApollWrapper
{
private:
	std::vector<struct pollfd> _events;

public:
	// constructor
	PollWrapper();
	PollWrapper(std::vector<Listener> listeners);
	~PollWrapper();

	// setter
	void addListener(int port);

	void addEvent(int fd, short event_flag);
	void modifyEvent(int fd, short event_flag);
	void removeEvent(int fd);

	// getter
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
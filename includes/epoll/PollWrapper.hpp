#pragma once

#include "ApollWrapper.hpp"
#include <poll.h>
#include <vector>

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
	bool is_listener(int index);
	bool is_pollin_event(int index);
	bool is_pollout_event(int index);
	bool is_timeout(int index);

	void accept(int index);
	void read(int index);
	void write(int index);
	void close(int index);
};
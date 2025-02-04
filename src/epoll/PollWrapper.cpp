#include "PollWrapper.hpp"

// constructor
PollWrapper::PollWrapper() {}

PollWrapper::PollWrapper(std::vector<Listener> listeners)
{
	_listeners = listeners;
	for (std::vector<Listener>::iterator i = _listeners.begin(); i != _listeners.end(); ++i)
	{
		addEvent(i->getFd(), POLLIN);
	}
}

PollWrapper::~PollWrapper() {}

// setter
void PollWrapper::addListener(int port)
{
	Listener listener(port);
	_listeners.push_back(listener);
	addEvent(listener.getFd(), POLLIN);
}

void PollWrapper::addEvent(int fd, short event_flag)
{
	struct pollfd new_event;
	new_event.fd = fd;
	new_event.events = event_flag;
	_events.push_back(new_event);
}

void PollWrapper::modifyEvent(int fd, short event_flag)
{
	for (std::vector<struct pollfd>::iterator i = _events.begin(); i != _events.end(); ++i)
	{
		if (i->fd == fd)
		{
			i->events = event_flag;
			break;
		}
	}
}

void PollWrapper::removeEvent(int fd)
{
	std::vector<struct pollfd> new_events;

	for (std::vector<struct pollfd>::iterator i = _events.begin(); i != _events.end(); ++i)
	{
		if (i->fd == fd)
			continue;
		new_events.push_back(*i);
	}

	_events.clear();
	_events = new_events;
}

// getter
int PollWrapper::getEventSize() const
{
	return _events.size();
}


// methods
void PollWrapper::wait()
{
	int ret;

	ret = poll(_events.data(), _events.size(), -1);
	if (ret < 0)
	{
		std::cerr << "Poll failed: " << strerror(errno) << std::endl;
		throw std::runtime_error("Poll failed");
	}

}

bool PollWrapper::is_listener(int index)
{
	if (!(_events[index].revents & POLLIN))
		return false;

	for (std::vector<Listener>::const_iterator i = _listeners.begin(); i != _listeners.end(); ++i)
	{
		if (i->getFd() == _events[index].fd)
			return true;
	}
	return false;
}

bool PollWrapper::is_pollin_event(int index)
{
	if (_events[index].revents & POLLIN)
		return true;
	return false;
}

bool PollWrapper::is_pollout_event(int index)
{
	if (_events[index].revents & POLLOUT)
		return true;
	return false;
}

void PollWrapper::accept(int index)
{
	Connection *newConn = new Connection(_events[index].fd);
	_connections.addConnection(newConn);
	addEvent(newConn->getFd(), POLLIN);
}

void PollWrapper::read(int index)
{
	Connection *conn = _connections.getConnection(_events[index].fd);
	try {
		conn -> readSocket();
	}
	catch (std::exception &e) {
		if (std::string(e.what()).find("Timed out") != std::string::npos)
			throw std::runtime_error(e.what());
		else
		{
			removeEvent(conn->getFd());
			return ;
		}
	}
	modifyEvent(conn->getFd(), POLLOUT);
}

void PollWrapper::write(int index)
{
	Connection *conn = _connections.getConnection(_events[index].fd);
	try {
		conn -> writeSocket();
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
	modifyEvent(conn->getFd(), POLLIN);
}
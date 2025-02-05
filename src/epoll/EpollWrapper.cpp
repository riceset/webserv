#include "EpollWrapper.hpp"

// constructor
EpollWrapper::EpollWrapper() {}

EpollWrapper::EpollWrapper(int max_events, std::vector<Listener> listeners)
{
	_listeners = listeners;
	epfd_ = epoll_create(max_events);
	if(epfd_ == -1)
		throw std::runtime_error(
			"[EpollWrapper] Failed to create epoll instance");
	for(std::vector<Listener>::iterator i = _listeners.begin();
		i != _listeners.end();
		++i)
	{
		addEvent(i->getFd(), EPOLLIN);
	}
}

EpollWrapper::~EpollWrapper() {}

// setter

void EpollWrapper::addListener(int port)
{
	Listener listener(port);
	_listeners.push_back(listener);
	addEvent(listener.getFd(), EPOLLIN);
}

void EpollWrapper::addEvent(int fd, uint32_t events)
{
	struct epoll_event new_event;
	new_event.events = events;
	new_event.data.fd = fd;
	if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &new_event) == -1)
		throw std::runtime_error(
			"[EpollWrapper] Failed to add event to epoll instance");
	_events.push_back(new_event);

	std::cout << "add event" << std::endl;
}

void EpollWrapper::modifyEvent(int fd, uint32_t events)
{
	struct epoll_event modify_event;
	modify_event.events = events;
	modify_event.data.fd = fd;

	if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &modify_event) == -1)
		throw std::runtime_error(
			"[EpollWrapper] Failed to modify event in epoll instance");

	for(std::vector<struct epoll_event>::iterator i = _events.begin();
		i != _events.end();
		++i)
	{
		if(i->data.fd == fd)
		{
			i->events = events;
			break;
		}
	}

	std::cout << "modify event" << std::endl;
}

void EpollWrapper::removeEvent(int fd)
{
	if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error(
			"[EpollWrapper] Failed to delete event from epoll instance");

	std::vector<struct epoll_event> new_events;
	for(std::vector<struct epoll_event>::iterator i = _events.begin();
		i != _events.end();
		++i)
	{
		if(i->data.fd == fd)
			continue;
		new_events.push_back(*i);
	}
	_events.clear();
	_events = new_events;
}

// getter

int EpollWrapper::getEpfd() const
{
	return epfd_;
}

int EpollWrapper::getEventSize() const
{
	return _detected_event_size;
}

// methods

void EpollWrapper::wait()
{
	_detected_events.clear();
	_detected_events = _events;
	_detected_event_size = 0;

	for(;;)
	{
		_detected_event_size = epoll_wait(
			epfd_, _detected_events.data(), _detected_events.size(), 0);
		if(_detected_event_size == 0)
		{
			continue;
		}
		else if(_detected_event_size == -1)
		{
			throw std::runtime_error("[EpollWrapper] Epoll_wait failed");
		}

		break;
	}

	return;
}

bool EpollWrapper::isListener(int index)
{
	if(!(_detected_events[index].events & EPOLLIN))
		return false;

	for(std::vector<Listener>::const_iterator i = _listeners.begin();
		i != _listeners.end();
		++i)
	{
		if(i->getFd() == _detected_events[index].data.fd)
			return true;
	}
	return false;
}

bool EpollWrapper::isPollinEvent(int index)
{
	if(!(_detected_events[index].events & EPOLLIN))
		return false;
	return true;
}

bool EpollWrapper::isPolloutEvent(int index)
{
	if(!(_detected_events[index].events & EPOLLOUT))
		return false;
	return true;
}

bool EpollWrapper::isTimeOut(int index)
{
	try
	{
		Connection *conn =
			_connections.getConnection(_detected_events[index].data.fd);
		if(conn->isTimeout())
			return true;
	}
	catch(std::exception &e)
	{
		return false;
	}
	return false;
}

void EpollWrapper::accept(int index)
{
	Connection *newConn;
	try
	{
		newConn = new Connection(_listeners[index].getFd());
	}
	catch(std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	_connections.addConnection(newConn);
	addEvent(newConn->getFd(), EPOLLIN);
}

void EpollWrapper::read(int index)
{
	Connection *conn =
		_connections.getConnection(_detected_events[index].data.fd);
	try
	{
		conn->readSocket();
	}
	catch(std::exception &e)
	{
		if(std::string(e.what()).find("Timed out") != std::string::npos)
			throw std::runtime_error(e.what());
		else
		{
			removeEvent(conn->getFd());
			_connections.removeConnection(conn->getFd());
			return;
		}
	}
	modifyEvent(conn->getFd(), EPOLLOUT);
}

void EpollWrapper::write(int index)
{
	Connection *conn =
		_connections.getConnection(_detected_events[index].data.fd);
	try
	{
		conn->writeSocket();
	}
	catch(std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	modifyEvent(conn->getFd(), EPOLLIN);
}

void EpollWrapper::closeSocket(int index)
{
	Connection *conn =
		_connections.getConnection(_detected_events[index].data.fd);
	removeEvent(conn->getFd());
	_connections.removeConnection(conn->getFd());
	shutdown(conn->getFd(), SHUT_RDWR);
}
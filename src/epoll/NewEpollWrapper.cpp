#include "NewEpollWrapper.hpp"

// constructor
EpollWrapper::EpollWrapper() {}

EpollWrapper::EpollWrapper(int max_events)
{
	epfd_ = epoll_create(max_events);
	if(epfd_ == -1)
		throw std::runtime_error("[EpollWrapper] Failed to create epoll instance");
	_events.resize(max_events);
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
		throw std::runtime_error("[EpollWrapper] Failed to add event to epoll instance");
}

void EpollWrapper::modifyEvent(int fd, uint32_t events)
{
	struct epoll_event modify_event;
	modify_event.events = events;
	modify_event.data.fd = fd;
	if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &modify_event) == -1)
		throw std::runtime_error("[EpollWrapper] Failed to modify event in epoll instance");
}

void EpollWrapper::removeEvent(int fd)
{
	if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error("[EpollWrapper] Failed to delete event from epoll instance");
}

// getter

int EpollWrapper::getEpfd() const
{
	return epfd_;
}

int EpollWrapper::getEventSize() const
{
	return _events.size();
}

// methods

void EpollWrapper::wait()
{
	int nfds = epoll_wait(epfd_, _events.data(), _events.size(), 0);
	if(nfds == -1)
		throw std::runtime_error("[EpollWrapper] Epoll_wait failed");
	return ;
}

bool EpollWrapper::is_listener(int index)
{
	if (!(_events[index].events & EPOLLIN))
		return false;

	for (std::vector<Listener>::const_iterator i = _listeners.begin(); i != _listeners.end(); ++i)
	{
		if (i->getFd() == _events[index].data.fd)
			return true;
	}
	return false;
}

bool EpollWrapper::is_pollin_event(int index)
{
	if (!(_events[index].events & EPOLLIN))
		return false;
	return true;
}

bool EpollWrapper::is_pollout_event(int index)
{
	if (!(_events[index].events & EPOLLOUT))
		return false;
	return true;
}

void EpollWrapper::accept(int index)
{
	Connection *newConn = new Connection(_listeners[index].getFd());
	_connection.addConnection(newConn);
	addEvent(newConn->getFd() , EPOLLIN);
}

void EpollWrapper::read(int index)
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
	modifyEvent(conn->getFd(), EPOLLOUT);
}

void EpollWrapper::write(int index)
{
	Connection *conn = _connections.getConnection(_events[index].fd);
	try {
		conn -> writeSocket();
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
	modifyEvent(conn->getFd(), EPOLLIN);
}

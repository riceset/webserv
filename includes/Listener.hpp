#ifndef LISTENER_HPP
# define LISTENER_HPP

#include <netinet/in.h>
#include <sys/socket.h>

class Listener {
	private:
		int m_fd;
		struct sockaddr_in m_addr;
		int m_port;
	public:
		Listener(int port);
		~Listener();
		int getFd() const;
};

#endif

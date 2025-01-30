#include <vector>
#include <poll.h>
#include "Connection.hpp"
#include "ConnectionWrapper.hpp"
#include "Listener.hpp"

std::vector<Listener> createServerSockets() {
	// ? memo : socket wrapper needed??
	std::vector<Listener> listeners;
	Listener listener = Listener(8080);

	listeners.push_back(listener);
	
	return listeners;
}

std::vector<struct pollfd> createPollFds(std::vector<Listener> listeners) {
	std::vector<struct pollfd> pollfds;
	struct pollfd pollfd;

	for (size_t i = 0; i < listeners.size(); i++) {
		pollfd.fd = listeners[i].getFd();
		pollfd.events = POLLIN;
		pollfds.push_back(pollfd);
	}
	return pollfds;
}

std::vector<struct pollfd> removePollFd(std::vector<struct pollfd> pollfds, int fd) {
	std::vector<struct pollfd> newPollfds;

	for (std::vector<struct pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); ++i) {
		if (i->fd == fd) {
			continue;
		}
		newPollfds.push_back(*i);
	}

	return newPollfds;
}

int main() {
	std::vector<Listener> listeners = createServerSockets();
	std::vector<struct pollfd> pollfds = createPollFds(listeners);
	ConnectionWrapper connections;
	Connection *newConn = NULL;

	for (;;) {
		int ret = poll(pollfds.data(), pollfds.size(), -1);
		if (ret < 0) {
			std::cerr << "Poll failed: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}

		for (std::vector<struct pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); ++i) {
			if (i->revents & POLLIN) {
				std::cout << "Event on fd = " << i->fd << std::endl;
				std::vector<Listener>::iterator j = listeners.begin();
				// if the event is on a listener socket, accept the connection
				while (j != listeners.end()) {
					if (i->fd == j->getFd()) {
						newConn = new Connection(j->getFd());
						connections.addConnection(newConn);
						break;
					}
					j++;
				}
				// if the event is on a connection socket, read from it
				if (j == listeners.end()) {
					// ? memo : when connection wrapper cannot find the connection
					Connection *conn = connections.getConnection(i->fd);
					try {
						conn -> readSocket();
						i->events = POLLOUT;
					} catch (std::exception &e) {
						std::cerr << "Read failed: " << e.what() << std::endl;
					}
				}
				break;
			} else if (i->revents & POLLOUT) {
				std::cout << "Writing to connection fd = " << i->fd << std::endl;
				Connection *conn = connections.getConnection(i->fd);
				try {
					conn -> writeSocket();
					pollfds = removePollFd(pollfds, i->fd);
				} catch (std::exception &e) {
					std::cerr << "Write failed: " << e.what() << std::endl;
				}
				break;
			}
		}

		if (newConn != NULL) {
			struct pollfd new_pollfd;
			new_pollfd.fd = newConn->getFd();
			new_pollfd.events = POLLIN;
			pollfds.push_back(new_pollfd);
			newConn = NULL;
		}
	}
}

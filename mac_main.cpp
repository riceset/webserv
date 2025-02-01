#include <vector>
#include <poll.h>
#include "Connection.hpp"
#include "ConnectionWrapper.hpp"
#include "Listener.hpp"

void addPollFD(std::vector<struct pollfd> &pollfds, int fd, int events) {
	struct pollfd pollfd;
	pollfd.fd = fd;
	pollfd.events = events;
	pollfds.push_back(pollfd);
}

void removePollFd(std::vector<struct pollfd> &pollfds, int fd) {
	std::vector<struct pollfd> newPollfds;

	for (std::vector<struct pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); ++i) {
		if (i->fd == fd) {
			continue;
		}
		newPollfds.push_back(*i);
	}

	pollfds.clear();
	pollfds = newPollfds;
}

bool is_listener(std::vector<Listener> &listeners, int fd) {
	for (std::vector<Listener>::iterator i = listeners.begin(); i != listeners.end(); ++i) {
		if (i->getFd() == fd) {
			return true;
		}
	}
	return false;
}

int main() {
	// 管理すべき構造体
	std::vector<struct pollfd> pollfds;
	ConnectionWrapper connections;
	std::vector<Listener> listeners;

	// 初期化
	Listener listener(80);
	addPollFD(pollfds, listener.getFd(), POLLIN);
	listeners.push_back(listener);

	for (;;) {
		
		std::cout << "========== Polling : " << pollfds.size() << std::endl;
		std::cout << "Pollfds: ";
		for (std::vector<struct pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); ++i) {
			std::cout << " " << i->fd;
		}
		std::cout << std::endl;

		int ret = poll(pollfds.data(), pollfds.size(), -1);
		if (ret < 0) {
			std::cerr << "Poll failed: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}

		for (std::vector<struct pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); ++i) {
			std::cout << "fd: " << i->fd << std::endl; //debug

			if (i->revents & POLLIN) { // 読み込み可能

				std::cout << "POLLIN" << std::endl; //debug

				if (is_listener(listeners, i->fd)) // passive socket (リッスンしているソケット)
				{

					std::cout << "is_listener" << std::endl; //debug

					Connection *newConn = new Connection(i->fd);
					connections.addConnection(newConn);
					addPollFD(pollfds, newConn->getFd() , POLLIN);

					break;
				}
				else // pair socket (ペアソケット)
				{

					std::cout << "is_pair_socket" << std::endl; //debug

					try {
						connections.getConnection(i->fd) -> readSocket(); // ペアソケットの取得と読み込み
						i->events = POLLOUT;
					} catch (std::exception &e) {
						std::cerr << "Read failed: " << e.what() << std::endl;
					}
					break;
				}
			} else if (i->revents & POLLOUT) { // 書き込み可能

				std::cout << "POLLOUT" << std::endl; //debug

				try {
					connections.getConnection(i->fd) -> writeSocket(); // ペアソケットの取得と書き込み
					removePollFd(pollfds, i->fd);
				} catch (std::exception &e) {
					std::cerr << "Write failed: " << e.what() << std::endl;
					exit(EXIT_FAILURE);
				}
				break;
			}
		}
	}
}

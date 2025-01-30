#include <vector>
#include <poll.h>
#include "Connection.hpp"
#include "ConnectionWrapper.hpp"
#include "Listener.hpp"

std::vector<Listener> createServerSockets() {
	// memo : socket は wrapper 不要な気もする
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

int main() {
	std::vector<Listener> listeners = createServerSockets();
	std::vector<struct pollfd> pollfds = createPollFds(listeners);
	ConnectionWrapper connections;
	Connection *newConn;


	for (;;) {
		std::cout << "Polling..." << std::endl;
		int ret = poll(pollfds.data(), pollfds.size(), -1);
		if (ret < 0) {
			perror("poll");
			exit(EXIT_FAILURE);
		}

		for (std::vector<struct pollfd>::iterator i = pollfds.begin(); i != pollfds.end(); ++i) {
			std::cout << "Polling on fd = " << i->fd << std::endl;
			if (i->revents & POLLIN) {
				std::cout << "Event on fd = " << i->fd << std::endl;
				std::vector<Listener>::iterator j = listeners.begin();
				// listenソケットへの接続の場合は新しいConnectionを作成
				while (j != listeners.end()) {
					if (i->fd == j->getFd()) {
						newConn = new Connection(j->getFd());
						connections.addConnection(newConn);
						break;
					}
					j++;
				}
				// listenソケット以外の場合はデータの読み込み
				if (j == listeners.end()) {
					// ? コネクションが見つからないケースはあり得るのか？
					std::cout << "Reading from connection fd = " << i->fd << std::endl;
					Connection *conn = connections.getConnection(i->fd);
					try {
						conn -> readSocket();
					} catch (std::exception &e) {
						std::cerr << "Read failed: " << e.what() << std::endl;
					}
				}
			} else if (i->revents & POLLOUT) {
				std::cout << "Writing to connection fd = " << i->fd << std::endl;
				Connection *conn = connections.getConnection(i->fd);
				conn -> writeSocket();
			}
		}
		pollfd new_pollfd;

		new_pollfd.fd = newConn->getFd();
		new_pollfd.events = POLLIN;
		pollfds.push_back(new_pollfd);
	}


}

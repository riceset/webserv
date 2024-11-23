#include "./includes/Listener.hpp"
#include "./includes/Connection.hpp"
#include <iostream>
#include <stdexcept>
#include <poll.h>
#include <vector>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
	try {
		//create a listener socket
		Listener listener(8080);

		//initialize the poll instance
		std::vector<pollfd> fds;
		std::unordered_map<int, Connection*> connections;
		pollfd listenerFd;
		listenerFd.fd = listener.getFd();
		listenerFd.events = POLLIN;
		fds.push_back(listenerFd);

		//main loop
		while (true) {
			//wait for events
			int num_events = poll(fds.data(), fds.size(), -1);
			if (num_events == -1) {
				throw std::runtime_error("Failed to poll for events");
			}

			for (size_t i = 0; i < fds.size(); ++i) {
				//accept new connections
				if (fds[i].revents & POLLIN) {
					if (fds[i].fd == listener.getFd()) {
						//accept a new client connection
						int clientFd = accept(listener.getFd(), nullptr, nullptr);
						if (clientFd == -1) {
							throw std::runtime_error("Failed to accept client connection");
						}
						//set the client socket to non-blocking
						fcntl(clientFd, F_SETFL, O_NONBLOCK);
						//add the client socket to the poll instance
						Connection* conn = new Connection(clientFd);
						connections[clientFd] = conn;
						//add the client socket to the poll instance
						pollfd clientPollFd;
						clientPollFd.fd = clientFd;
						clientPollFd.events = POLLIN;
						fds.push_back(clientPollFd);
						std::cout << "New client connected: FD " << clientFd << std::endl;
					}
					else {
						//read data from the client
						int clientFd = fds[i].fd;
						Connection* conn = connections[clientFd];

						try {
							conn->readData();
							conn->processRequest();
							//change the event to write
							fds[i].events = POLLOUT;
						} catch (std::exception &e) {
							std::cerr << e.what() << std::endl;
							//remove the client socket from the poll instance
							close(clientFd);
							delete conn;
							connections.erase(clientFd);
							fds.erase(fds.begin() + i);
							--i;
						}
					}
				} else if(fds[i].revents & POLLOUT)  {
					//write data to the client
					int clientFd = fds[i].fd;
					Connection* conn = connections[clientFd];

					try {
						conn->writeData();
						//remove the POLLOUT event
						close(clientFd);
						delete conn;
						connections.erase(clientFd);
						fds.erase(fds.begin() + i);
						--i;
					} catch (std::exception &e) {
						std::cerr << e.what() << std::endl;
						//remove the client socket from the poll instance
						close(clientFd);
						delete conn;
						connections.erase(clientFd);
						fds.erase(fds.begin() + i);
						--i;
					}
				} 
			}
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

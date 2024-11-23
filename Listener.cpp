#include "./includes/Listener.hpp"
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>


//Listen socket constructor
Listener::Listener(int port) : m_port(port) {

	//Create a Listen socket
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_fd == -1) {
		throw std::runtime_error("Failed to create socket");
	}

	//Initialize the socket address structure
	bzero(&m_addr, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(m_port);

	//Bind the socket to the address
	if (bind(m_fd, (struct sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
		throw std::runtime_error("Failed to bind socket");
	}

	//Start listening
	if (listen(m_fd, SOMAXCONN) == -1) {
		throw std::runtime_error("Failed to listen on socket");
	}

	int flags = fcntl(m_fd, F_GETFL, 0);
	if (flags == -1) {
		throw std::runtime_error("Failed to get socket flags");
	}
	if (fcntl(m_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw std::runtime_error("Failed to set socket to non-blocking");
	}
	std::cout << "Listening on port " << m_port << std::endl;
}

//Listen socket destructor
Listener::~Listener() {
	if (m_fd != -1)
		close(m_fd);
}

//Get the file descriptor of the listen socket
int Listener::getFd() const {
	return m_fd;
}

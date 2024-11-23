#include "./includes/Listener.hpp"
#include "./includes/Connection.hpp"
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>


Connection::Connection(int fd) : m_clientFd(fd) {};

Connection::~Connection() {
	if (m_clientFd != -1)
		close(m_clientFd);
}

int Connection::getSocket() const {
	return m_clientFd;
}

void Connection::readData() {
	char buf[1024];
	size_t bytes = recv(m_clientFd, buf, sizeof(buf) - 1, 0);
	if (bytes > 0) {
		buf[bytes] = '\0';
		std::cout << buf << std::endl;
		m_clientbuf += buf;
	} else if (bytes == 0) {
		throw std::runtime_error("Client disconnected");
	} else {
		throw std::runtime_error("Failed to read from client");
	}
}

void Connection::writeData() {
	ssize_t bytes = send(m_clientFd, m_serverbuf.c_str(), m_serverbuf.size(), 0);
	std::cout << m_serverbuf << std::endl;
	if (bytes == -1) {
		throw std::runtime_error("Failed to write to client");
	}
	m_serverbuf.erase(0, bytes);
}

void Connection::processRequest() {
	if (m_clientbuf.find("GET") != 0) {
		m_serverbuf = "HTTP/1.1 400 Bad Request\r\n\r\n";
		return;
	}
	//parse the request
	size_t start = m_clientbuf.find(" ") + 1;
	size_t end = m_clientbuf.find(" ", start);
	std::string path = m_clientbuf.substr(start, end - start);
	//setting the root directory
	std::string root = "./www";
	if (path == "/") {
		path = "/index.html";
	}
	std::string full_path = root + path;
	//check whether the file exists
	if (std::filesystem::exists(full_path) && std::filesystem::is_regular_file(full_path)) {
		//read the file
		std::ifstream file(full_path);
		if (!file) {
			m_serverbuf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
			return;
		}
		std::ostringstream content_stream;
		content_stream << file.rdbuf();
		std::string content = content_stream.str();
		//make the response
		m_serverbuf = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Length: " + std::to_string(content.size()) + "\r\n"
			"Content-Type: text/html\r\n"
			"\r\n" + content;
	} else {
		std::fstream file("./www/404.html");
		std::ostringstream content_stream;
		content_stream << file.rdbuf();
		std::string content = content_stream.str();
		m_serverbuf = 
			"HTTP/1.1 404 Not Found\r\n"
			"Content-Length: " + std::to_string(content.size()) + "\r\n"
			"Content-Type: text/html\r\n"
			"\r\n" + content;
	}
}


















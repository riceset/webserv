#include "CGI.hpp"

CGI::CGI() : _fd(-1), _path("") {}

CGI::CGI(std::string path) {
	std::cout << "[cgi] cgi has called with path: " << path << std::endl;
	int status;
	int pipe_fd[2];

	if(pipe(pipe_fd) == -1) {
		std::cout << "[cgi] pipe failed" << std::endl;
		throw std::runtime_error("[cgi] pipe failed");
	}

	_pid = fork();
	if(_pid == -1) {
		std::cout << "[cgi] fork failed" << std::endl;
		throw std::runtime_error("[cgi] fork failed");
	}
	if(_pid == 0) {
		const char *php = "/usr/bin/php";
		const char *script = path.c_str();
		char *const args[] = {(char *)php, (char *)script, NULL};
		char *const envp[] = {NULL};

		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		std::cout << "HTTP/1.1 200 OK\r\n";
		std::cout << "Date: Mon, 25 Feb 2025 12:34:56 GMT\r\n";
		std::cout << "Content-Type: text/html; charset=UTF-8\r\n";
		std::cout << "Content-Length: 14\r\n"; // Content-Length を指定
		std::cout << "Connection: keep-alive\r\n";
		std::cout << "\r\n";
		execve(php, args, envp);
	} else {
		close(pipe_fd[1]);
		int flags = fcntl(pipe_fd[0], F_GETFL, 0);
		if(flags == -1) {
			std::cout << "[cgi] fcntl failed on pipe_fd[0]" << std::endl;
			throw std::runtime_error("[cgi] fcntl failed");
		}
		fcntl(pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
		_fd = pipe_fd[0];
	}

	waitpid(_pid, &status, 0);
}

CGI::CGI(const CGI &cgi) {
	_fd = cgi._fd;
	_path = cgi._path;
}

CGI::~CGI() {
	close(_fd);
}

CGI &CGI::operator=(const CGI &cgi) {
	if(this == &cgi)
		return *this;
	_fd = cgi._fd;
	_path = cgi._path;
	return *this;
}

// ============= getter =============

int CGI::getFd() const {
	return _fd;
}

void CGI::killCGI() {
	kill(_pid, SIGKILL);
}
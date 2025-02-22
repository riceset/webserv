#include "CGI.hpp"

CGI::CGI() : _fd(-1), _path("")
{
}

CGI::CGI(std::string path)
{
	int pipe_fd[2];

	_pid = fork();
	if (_pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
		execve(path.c_str(), NULL, NULL); //テスト用だな
		perror("execve");
	}
	else
	{
		close(pipe_fd[1]);
		_fd = pipe_fd[0];
	}

}

CGI::CGI(const CGI &cgi)
{
	_fd = cgi._fd;
	_path = cgi._path;
}

CGI::~CGI()
{
	close(_fd);
}

CGI &CGI::operator=(const CGI &cgi)
{
	if (this == &cgi)
		return *this;
	_fd = cgi._fd;
	_path = cgi._path;
	return *this;
}

// ============= getter =============

int CGI::getFd() const
{
	return _fd;
}

void CGI::killCGI()
{
	kill(_pid, SIGKILL);
	close(_fd);
}
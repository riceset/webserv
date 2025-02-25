#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>

// int main()
// {
// 	// std::string error_page = "." + conf_value_._root + conf_value_._error_page.back();
// 	std::string error_page = "/home/atokamot/git-cursus/webserv/www/404.html"; // テスト用

// 	std::cout << "[connection] error_page: " << error_page << " is set" << std::endl;
// 	int fd = open(error_page.c_str(), O_RDONLY);
// 	// ! エラーハンドリング
//     if (fd == -1)
//     {
//         std::cerr << "open failed" << std::endl;
//         return 1;
//     }
//     char buff[1024];
//     ssize_t rlen = read(fd, buff, 1024);
//     if (rlen == -1)
//     {
//         std::cerr << "read failed" << std::endl;
//         return 1;
//     }
//     buff[rlen] = '\0';
//     std::cout << buff << std::endl;
//     close(fd);
//     return 0;
// }

int cgi() {
	std::string path = "./www/index.php";
	std::cout << "[cgi] cgi has called with path: " << path << std::endl;
	int pipe_fd[2];

	if (pipe(pipe_fd) == -1)
	{
		std::cout << "[cgi] pipe failed" << std::endl;
		throw std::runtime_error("[cgi] pipe failed");
	}

	int _pid;
	int _fd;
	int status;

	_pid = fork();
	if (_pid == -1)
	{
		std::cout << "[cgi] fork failed" << std::endl;
		throw std::runtime_error("[cgi] fork failed");
	}
	if (_pid == 0)
	{
		const char *php = "/usr/bin/php";
		const char *script = path.c_str();
		char *const args[] = { (char *)php, (char *)script, NULL };
		char *const envp[] = { NULL };

		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		execve(php, args, envp);
	}
	// 子プロセスの終了を待つ
	waitpid(_pid, &status, 0);

	close(pipe_fd[1]);
	// パイプのファイルディスクリプタにO_NONBLOCKを設定
	int flags = fcntl(pipe_fd[0], F_GETFL, 0);
	if (flags == -1) {
		std::cout << "[cgi] fcntl failed on pipe_fd[0]" << std::endl;
		throw std::runtime_error("[cgi] fcntl failed");
	}
	fcntl(pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
	_fd = pipe_fd[0];

	return _fd;
}

int main() {
	int _fd = cgi();

	// テスト用
	char buff[1000];
	int r = read(_fd, buff, 3);
	if (r == -1)
	{
		std::cout << "[cgi] read failed" << std::endl;
		throw std::runtime_error("[cgi] read failed");
	}
	else
	{
		buff[r] = '\0';
		std::cout << "[cgi] read size = " << r << " read: " << buff << std::endl;
	}

	// テスト epollに登録してみる
	int epfd = epoll_create(1);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, _fd, &ev);

	// epollで待つ
	struct epoll_event events[1];
	int nfds = epoll_wait(epfd, events, 1, 0);
	if (nfds == -1)
	{
		std::cout << "[cgi] epoll_wait failed" << std::endl;
		throw std::runtime_error("[cgi] epoll_wait failed");
	}
	else if (nfds == 0)
	{
		std::cout << "[cgi] epoll_wait timeout" << std::endl;
	}
	else
	{
		std::cout << "[cgi] epoll_wait success" << std::endl;
		r = read(events[0].data.fd, buff, 100);
		buff[r] = '\0';
		std::cout << "[cgi] read size = " << r << " read: " << buff << std::endl;
	}
}

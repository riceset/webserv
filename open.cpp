#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	// std::string error_page = "." + conf_value_._root + conf_value_._error_page.back();
	std::string error_page = "/home/atokamot/git-cursus/webserv/www/404.html"; // テスト用

	std::cout << "[connection] error_page: " << error_page << " is set" << std::endl;
	int fd = open(error_page.c_str(), O_RDONLY);
	// ! エラーハンドリング
    if (fd == -1)
    {
        std::cerr << "open failed" << std::endl;
        return 1;
    }
    char buff[1024];
    ssize_t rlen = read(fd, buff, 1024);
    if (rlen == -1)
    {
        std::cerr << "read failed" << std::endl;
        return 1;
    }
    buff[rlen] = '\0';
    std::cout << buff << std::endl;
    close(fd);
    return 0;
}


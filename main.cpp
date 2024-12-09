#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <cstdio>

int main() {
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return 1;
    }
    std::cout << "epoll instance created successfully!" << std::endl;
    close(epfd);
    return 0;
}


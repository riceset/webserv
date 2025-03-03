#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

class CGI
{
private:
    std::string _scriptPath;

    void createPipe(int pipefd[2]);
    pid_t createChildProcess();
    void executeScriptInChild(int pipefd[2]);
    std::string readScriptOutput(int pipefd[2]);

public:
    CGI(const std::string& scriptPath);
    ~CGI();

    std::string execute();
};

#endif

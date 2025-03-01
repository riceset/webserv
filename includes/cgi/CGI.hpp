#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <map>
#include <vector>
#include <cstring>

#include "../http/HttpRequest.hpp"
#include "../config/BaseConf.hpp"

class CGI
{
private:
    std::string _scriptPath;
    std::string _method;

public:
    CGI(const std::string& scriptPath);

    CGI(const std::string& scriptPath, const std::string& method);

    std::string execute(const HttpRequest *request, const conf_value_t& conf_value);

    ~CGI();

private:
    void createPipe(int pipefd[2]);
    pid_t createChildProcess();
    void executeScriptInChild(int pipefd[2], char **envp);
    std::string readScriptOutput(int pipefd[2]);
    std::string parseOutput(const std::string& rawOutput);

    std::string headerNameToEnvFormat(const std::string& headerName);
};

#endif
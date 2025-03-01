#include "CGI.hpp"

CGI::CGI(const std::string &scriptPath)
    : _scriptPath(scriptPath), _method("GET") {}

CGI::CGI(const std::string& scriptPath, const std::string& method) 
    : _scriptPath(scriptPath), _method(method) {}

void CGI::createPipe(int pipefd[2]) {
    if (pipe(pipefd) == -1)
        throw std::runtime_error("pipe");
}

pid_t CGI::createChildProcess() {
    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("fork");
    return pid;
}

void CGI::executeScriptInChild(int pipefd[2], char **envp) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    char* args[] = {(char*)"/usr/bin/php", (char*)_scriptPath.c_str(), NULL};
    execve(args[0], args, envp);

    throw std::runtime_error("execve");
}

std::string CGI::readScriptOutput(int pipefd[2]) {
    close(pipefd[1]);

    char buffer[1024];
    std::string output;
    ssize_t bytesRead;

    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }
    close(pipefd[0]);
    return output;
}

/*
    Produces: Content-Type -> CONTENT_TYPE
*/
std::string CGI::headerNameToEnvFormat(const std::string &headerName) {
    std::string result;

    for (size_t i = 0; i < headerName.size(); ++i) {
        char c = headerName[i];
        if (c == '-')
            result += '_';
        else
            result += std::toupper(c);
    }

    return result;
}

std::string CGI::parseOutput(const std::string& rawOutput) {
    size_t headerEnd = rawOutput.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        return "Content-Type: text/html\r\n\r\n" + rawOutput;
    }
    
    return rawOutput;
}

std::string CGI::execute(const HttpRequest *request, const conf_value_t& conf_value) {
    int pipefd[2];
    pid_t pid;
    std::string output;

    std::map<std::string, std::string> env;

    env["REQUEST_METHOD"] = _method;
    env["CONTENT_TYPE"] = request->getHeader().count("Content-Type") ? request->getHeader().at("Content-Type") : "";
    env["CONTENT_LENGTH"] = request->getHeader().count("Content-Length") ? request->getHeader().at("Content-Length") : "";

    env["SERVER_NAME"] = conf_value._server_name;
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["SERVER_PORT"] = conf_value._listen;

    for (std::map<std::string, std::string>::const_iterator it = request->getHeader().begin(); 
    it != request->getHeader().end(); ++it) {
        if (it->first != "Content-Type" && it->first != "Content-Length") {
       env["HTTP_" + headerNameToEnvFormat(it->first)] = it->second;
        }
    }

    std::vector<std::string> envStrings;
    for (std::map<std::string, std::string>::const_iterator it = env.begin(); 
         it != env.end(); ++it) {
        envStrings.push_back(it->first + "=" + it->second);
    }
    
    char** envp = new char*[envStrings.size() + 1];
    for (size_t i = 0; i < envStrings.size(); ++i) {
        envp[i] = new char[envStrings[i].size() + 1];
        strcpy(envp[i], envStrings[i].c_str());
    }
    envp[envStrings.size()] = NULL;

    try {
        createPipe(pipefd);
        pid = CGI::createChildProcess();
        if (pid == 0) {
            CGI::executeScriptInChild(pipefd, envp);
        } else {
            output = CGI::readScriptOutput(pipefd);
            waitpid(pid, NULL, 0);
        }
    } catch (const std::exception& e) {
        if (pid == 0)
            _exit(1); 
        throw;
    }

    for (size_t i = 0; i < envStrings.size(); ++i) {
        delete[] envp[i];
    }
    delete[] envp;

    return output;
}

CGI::~CGI() {}
#include "CGI.hpp"

CGI::CGI(const std::string &scriptPath) : _scriptPath(scriptPath) {}

std::string CGI::execute()
{
    return "CGI";
}

CGI::~CGI() {}
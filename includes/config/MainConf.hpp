#pragma once

#include <map>
#include <sstream>

#include "BaseConf.hpp"
#include "ServConf.hpp"

class MainConf : public BaseConf
{
private:
	// server
	std::vector<ServConf> _servers;

	// handler
	typedef void (MainConf::*handler_directive_t)(std::vector<std::string>);
	std::map<std::string, handler_directive_t> _handler_directive;

public:
	MainConf(std::string content);
	~MainConf();

	// setter
	virtual void param(std::string conf_content);

	// getter
	// port number, server_name, path (3 arguments) finds the corresponding
	// conf_value_t
	conf_value_t getConfValue(std::string port,
							  std::string server_name,
							  std::string path); // throw
	// listen portを取得 passive socket 作成に利用
	std::vector<int> get_listen(); // throw

	// handler
	void handle_server_block(std::vector<std::string> tokens);

	// debug
	void debug_print();
	void debug_print_conf_value(conf_value_t conf_value);
};

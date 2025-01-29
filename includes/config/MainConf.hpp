#pragma once
#include <map>

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

	// 値の設定
	virtual void param(std::string conf_content);

	// handler
	void handle_server_block(std::vector<std::string> tokens);

	// debug
	void debug_print();
};

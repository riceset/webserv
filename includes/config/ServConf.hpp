#pragma once
#include <iostream>
#include <map>

#include "BaseConf.hpp"
#include "LocConf.hpp"

class ServConf : public BaseConf
{
private:
	// value
	std::string _listen;
	std::string _server_name;
	std::vector<std::string> _error_page;
	std::string _client_max_body_size;

	// location
	std::vector<LocConf> _locations;

	// handler
	std::map<std::string, void (ServConf::*)(std::vector<std::string>)>
		_handler_directive;

public:
	ServConf(std::string content);
	~ServConf();

	virtual void param(std::string content);

	// set methods
	void set_listen(std::vector<std::string> tokens);
	void set_server_name(std::vector<std::string> tokens);
	void set_error_page(std::vector<std::string> tokens);
	void set_client_max_body_size(std::vector<std::string> tokens);
	void handle_location_block(std::vector<std::string> tokens);

	// gettter
	std::string get_listen();
	std::string get_server_name();

	// get conf_value_t
	conf_value_t getConfValue(std::string path);

	// debug
	void debug_print();
};

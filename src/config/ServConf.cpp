#include "ServConf.hpp"

// Constructor
ServConf::ServConf(std::string content)
{
	// init
	_locations.clear();
	_handler_directive["location"] = &ServConf::handle_location_block;
	_handler_directive["listen"] = &ServConf::set_listen;
	_handler_directive["server_name"] = &ServConf::set_server_name;
	_handler_directive["error_page"] = &ServConf::set_error_page;
	_handler_directive["client_max_body_size"] =
		&ServConf::set_client_max_body_size;

	_listen.clear();
	_server_name.clear();
	_error_page.clear();
	_client_max_body_size.clear();
	_locations.clear();

	param(content);
}

// Destructor
ServConf::~ServConf() {}

// Setter
void ServConf::param(std::string content)
{
	size_t pos = 0;

	while(1)
	{
		std::vector<std::string> tokens;

		try
		{
			int result = BaseConf::parse_token(content, tokens, pos);
			if(result == CONF_ERROR)
			{
				throw std::runtime_error("token error");
			}
			if(result == CONF_EOF)
			{
				break;
			}
		}
		catch(std::runtime_error &e)
		{
			throw std::runtime_error("token error");
		}

		if(tokens.empty())
		{
			continue;
		}

		if(_handler_directive.find(tokens[0]) != _handler_directive.end())
		{
			(this->*_handler_directive[tokens[0]])(tokens);
		}
	}
}

void ServConf::set_listen(std::vector<std::string> tokens)
{
	if(tokens.size() != 2)
	{
		throw std::runtime_error("set_string error");
	}

	if(_listen.empty())
	{
		_listen = tokens[1];
	}
	else
	{
		throw std::runtime_error("listen duplicate error");
	}
}

void ServConf::set_server_name(std::vector<std::string> tokens)
{
	if(tokens.size() != 2)
	{
		throw std::runtime_error("set_string error");
	}

	if(_server_name.empty())
	{
		_server_name = tokens[1];
	}
	else
	{
		throw std::runtime_error("server_name duplicate error");
	}
}

void ServConf::set_error_page(std::vector<std::string> tokens)
{
	if(tokens.size() < 2)
	{
		throw std::runtime_error("set_array error");
	}
	if(_error_page.empty())
	{
		for(size_t i = 1; i < tokens.size(); i++)
		{
			_error_page.push_back(tokens[i]);
		}
	}
	else
	{
		throw std::runtime_error("error_page duplicate error");
	}
}

void ServConf::set_client_max_body_size(std::vector<std::string> tokens)
{
	if(tokens.size() != 2)
	{
		throw std::runtime_error("set_string error");
	}

	if(_client_max_body_size.empty())
	{
		_client_max_body_size = tokens[1];
	}
	else
	{
		throw std::runtime_error("client_max_body_size duplicate error");
	}
}

// handle block
void ServConf::handle_location_block(std::vector<std::string> tokens)
{
	if(tokens.size() != 3)
	{
		throw std::runtime_error("handle_location_block error");
	}

	// trim { }
	tokens[2].erase(0, 1);
	tokens[2].erase(tokens[2].size() - 1, 1);

	// location block, path
	_locations.push_back(LocConf(tokens[2], tokens[1]));
}

// Getter
std::string ServConf::get_listen()
{
	return _listen;
}

std::string ServConf::get_server_name()
{
	return _server_name;
}

// get conf_value_t
LocConf select_location(std::string path, std::vector<LocConf> locations)
{
	size_t max_len = 0;
	LocConf locConf;

	for(size_t i = 0; i < locations.size(); i++)
	{
		// pathの前方一致の中で長いものを優先
		// /hoge/fuga と /hoge がある場合、/hoge/fuga が優先される
		if(max_len < path.size() &&
		   !std::strncmp(locations[i].get_path().c_str(),
						 path.c_str(),
						 locations[i].get_path().size()))
		{
			max_len = path.size();
			locConf = locations[i];
		}
	}

	if(locConf.get_path().empty())
	{
		throw std::runtime_error("[server] location not found");
	}

	return locConf;
}

conf_value_t ServConf::get_conf_value(std::string path)
{
	conf_value_t conf_value;
	LocConf locConf;

	conf_value._listen = _listen;
	conf_value._server_name = _server_name;
	conf_value._error_page = _error_page;
	conf_value._client_max_body_size = _client_max_body_size;

	try
	{
		locConf = select_location(path, _locations);
	}
	catch(std::runtime_error &e)
	{
		throw std::runtime_error("[server] location not found");
	}

	locConf.get_conf_value(path, conf_value);
	return conf_value;
}

// debug
void ServConf::debug_print()
{
	std::cout << "listen: " << _listen << std::endl;
	std::cout << "server_name: " << _server_name << std::endl;
	std::cout << "error_page: ";

	for(size_t i = 0; i < _error_page.size(); i++)
	{
		std::cout << _error_page[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "client_max_body_size: " << _client_max_body_size << std::endl;

	for(size_t i = 0; i < _locations.size(); i++)
	{
		_locations[i].debug_print();
	}

	std::cout << std::endl;
}

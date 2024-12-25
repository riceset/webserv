#include "LocConf.hpp"

LocConf::LocConf(std::string content, std::string path) : _path(path) {
	// init
	_locations.clear();
	_limit_except.clear();
	_return.clear();
	_autoindex = false;
	_index.clear();
	_root.clear();
	_client_max_body_size.clear();

	// init handler directive
	_handler_directive["limit_except"] = &LocConf::set_limit_except;
	_handler_directive["return"] = &LocConf::set_return;
	_handler_directive["autoindex"] = &LocConf::set_autoindex;
	_handler_directive["index"] = &LocConf::set_index;
	_handler_directive["root"] = &LocConf::set_root;
	_handler_directive["client_max_body_size"] = &LocConf::set_client_max_body_size;
	_handler_directive["location"] = &LocConf::handle_location_block;

	param(content);
}

LocConf::~LocConf() {

}

void LocConf::param(std::string content) {
	size_t pos = 0;

	while (1) {
		std::vector<std::string> tokens;

		try {
			int result = BaseConf::parse_token(content, tokens, pos);
			if (result == CONF_EOF) {
				break;
			}
		}
		catch (std::runtime_error &e) {
			throw std::runtime_error("token error");
		}

		if (tokens.empty()) {
			continue;
		}

		if (_handler_directive.find(tokens[0]) != _handler_directive.end()) {
			try {
				(this->*_handler_directive[tokens[0]])(tokens);
			}
			catch (std::runtime_error &e) {
				throw std::runtime_error("directive error");
			}
		}
	}
}


void LocConf::handle_location_block(std::vector<std::string> tokens) {
	// tokens[0] = location
	// tokens[1] = path
	// tokens[2] = { ... }
	if (tokens.size() != 3) {
		throw std::runtime_error("location block syntax error");
	}

	// trim { }
	tokens[2].erase(0, 1);
	tokens[2].erase(tokens[2].size() - 1, 1);

	_locations.push_back(LocConf(tokens[2], tokens[1]));
}

void LocConf::set_limit_except(std::vector<std::string> tokens) {
	if (_limit_except.size() > 0) {
		throw std::runtime_error("limit_except already set");
	}
	if (tokens.size() < 2) {
		throw std::runtime_error("limit_except syntax error");
	}

	size_t i = 1;
	while (tokens.size() > i) {
		// 小文字化
		std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);

		// メソッド名のチェック
		// if (tokens[i] != "get" && tokens[i] != "post" && tokens[i] != "delete" && tokens[i] != "put") {
		// 	throw std::runtime_error("limit_except syntax error");
		// }
		_limit_except.push_back(tokens[i]);
		i++;
	}
}

void LocConf::set_return(std::vector<std::string> tokens) {
	if (_return.size() > 0) {
		throw std::runtime_error("return already set");
	}
	if (tokens.size() < 2) {
		throw std::runtime_error("return syntax error");
	}

	size_t i = 1;
	while (tokens.size() > i) {
		_return.push_back(tokens[i]);
		i++;
	}
}

void LocConf::set_autoindex(std::vector<std::string> tokens) {
	if (_autoindex) {
		throw std::runtime_error("autoindex already set");
	}
	if (tokens.size() < 2) {
		throw std::runtime_error("autoindex syntax error");
	}

	if (tokens[1] == "on") {
		_autoindex = true;
	}
	else if (tokens[1] == "off") {
		_autoindex = false;
	}
	else {
		throw std::runtime_error("autoindex syntax error");
	}
}

void LocConf::set_index(std::vector<std::string> tokens) {
	if (_index.size() > 0) {
		throw std::runtime_error("index already set");
	}
	if (tokens.size() < 2) {
		throw std::runtime_error("index syntax error");
	}

	size_t i = 1;
	while (tokens.size() > i) {
		_index.push_back(tokens[i]);
		i++;
	}
}

void LocConf::set_root(std::vector<std::string> tokens) {
	if (_root.size() > 0) {
		throw std::runtime_error("root already set");
	}
	if (tokens.size() < 2) {
		throw std::runtime_error("root syntax error");
	}

	_root = tokens[1];
}

void LocConf::set_client_max_body_size(std::vector<std::string> tokens) {
	if (_client_max_body_size.size() > 0) {
		throw std::runtime_error("client_max_body_size already set");
	}
	if (tokens.size() < 2) {
		throw std::runtime_error("client_max_body_size syntax error");
	}

	_client_max_body_size = tokens[1];
}

// debug
void LocConf::debug_print() {
	std::cout << std::endl;
	std::cout << "path: " << _path << std::endl;
	std::cout << "limit_except: ";
	std::cout << "return: ";
	for (size_t i = 0; i < _return.size(); i++) {
		std::cout << _return[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "autoindex: " << _autoindex << std::endl;
	std::cout << "index: ";
	for (size_t i = 0; i < _index.size(); i++) {
		std::cout << _index[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "root: " << _root << std::endl;
	std::cout << "client_max_body_size: " << _client_max_body_size << std::endl;
	std::cout << "locations: " << _locations.size() << std::endl;
	for (size_t i = 0; i < _locations.size(); i++) {
		std::cout << "location " << i << ":" << std::endl;
		_locations[i].debug_print();
	}
	std::cout << std::endl << std::endl;
}

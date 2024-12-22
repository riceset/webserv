#include "MainConf.hpp"

// 全ての値を初期化
MainConf::MainConf(std::string conf_content) : _servers() {
	// init
	_servers.clear();
	_handler_directive["server"] = &MainConf::handle_server_block;

	param(conf_content);
}

MainConf::~MainConf() {
}

void MainConf::param(std::string conf_content) {
	size_t pos = 0;

	while (1) {
		std::vector<std::string> tokens;

		try {
			int result = BaseConf::parse_token(conf_content, tokens, pos);
			if (result == CONF_ERROR) {
				throw std::runtime_error("token error");
			}
			if (result == CONF_EOF) {
				break;
			}
		}
		catch (std::runtime_error &e) {
			throw std::runtime_error("token error");
		}

		if (!tokens.empty() && _handler_directive.find(tokens[0]) != _handler_directive.end()) {
			(this->*_handler_directive[tokens[0]])(tokens);
		}
	}
}

void MainConf::handle_server_block(std::vector<std::string> tokens) {
	if (tokens.size() != 2) {
		throw std::runtime_error("server block syntax error");
	}
	// tokens[0] = server
	// tokens[1] = { ... }

	// trim { }
	tokens[1].erase(0, 1);
	tokens[1].erase(tokens[1].size() - 1, 1);

	_servers.push_back(ServConf(tokens[1]));
}

void MainConf::debug_print() {
	for (size_t i = 0; i < _servers.size(); i++) {
		std::cout << "server " << i << ":" << std::endl;
		_servers[i].debug_print();
	}
}

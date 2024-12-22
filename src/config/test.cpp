#include "MainConf.hpp"
#include <fstream>

// テスト用に使用

std::string read_file(std::string file_path) {
	std::ifstream file(file_path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file");
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

int main(int /*argc*/, char **argv) {
	if (argv[1] == NULL) {
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}
	std::string conf_content = read_file(argv[1]);

	MainConf main_conf(conf_content);

	main_conf.debug_print();
}

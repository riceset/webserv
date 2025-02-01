#include <fstream>

#include "LocConf.hpp"
#include "MainConf.hpp"
#include "ServConf.hpp"

std::string get_file_content(std::string filename)
{
	std::ifstream ifs(filename);
	if(!ifs)
	{
		std::cerr << "Error: file not found" << std::endl;
		exit(1);
	}

	std::string content;
	while(!ifs.eof())
	{
		std::string line;
		std::getline(ifs, line);
		content += line + "\n";
	}

	return content;
}

int main()
{
	std::string content;
	content = get_file_content("test.conf");

	// std ::cout << content << std::endl;

	MainConf mainConf(content);
	// search server && location block
	std::string port = "8080";
	std::string server_name = "localhost";
	std::string path = "/under_construction/";
	// std::string path = "/cgi/";
	conf_value_t conf_value;

	try {
		conf_value = mainConf.get_conf_value(port, server_name, path);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	// debug
	mainConf.debug_print_conf_value(conf_value);

	return 0;
}
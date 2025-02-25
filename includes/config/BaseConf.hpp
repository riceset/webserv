#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class BaseConf
{
protected:
	static const int NORMAL = 0;
	static const int IN_DOUBLE_QUOTE = 1;
	static const int IN_SINGLE_QUOTE = 2;
	static const int IN_BRACE = 3;

	static const int CONF_EOF = 0;
	static const int CONF_ERROR = 1;
	static const int DIRECTIVE_OK = 2;
	static const int BLOCK_OK = 3;

public:
	virtual ~BaseConf() {}

	// 純粋仮想関数
	virtual void param(std::string content) = 0;

protected:
	// 共通のトークン解析メソッド
	int parse_token(std::string conf_content,
					std::vector<std::string> &tokens,
					size_t &pos);

private:
	void reset_token_state(int &state,
						   std::string &token,
						   std::vector<std::string> &tokens)
	{
		state = NORMAL;
		tokens.push_back(token);
		token.clear();
	}
};

struct conf_value_t {
	std::string _listen;
	std::string _server_name;
	std::vector<std::string> _error_page;
	std::string _path;
	std::vector<std::string> _limit_except;
	std::vector<std::string> _return;
	bool _autoindex;
	std::vector<std::string> _index;
	std::string _root;
	std::string _client_max_body_size;
};

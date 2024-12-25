#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#define CONF_EOF 0
#define CONF_ERROR 1
#define DIRECTIVE_OK 2
#define BLOCK_OK 3

#define NORMAL 0
#define IN_DOUBLE_QUOTE 1
#define IN_SINGLE_QUOTE 2
#define IN_BRACE 3

class BaseConf {
public:
	BaseConf();
	virtual ~BaseConf();
	virtual void param(std::string content) = 0;

protected:
	// 共通のトークン解析メソッド
	int parse_token(std::string conf_content, std::vector<std::string> &tokens, size_t &pos);
	void reset_token_state(int &state, std::string &token, std::vector<std::string> &tokens);

private:
};
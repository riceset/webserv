#include "BaseConf.hpp"

int BaseConf::parse_token(std::string conf_content, std::vector<std::string> &tokens, size_t &pos) {
    std::string token;
    char c;
    
    int state = NORMAL;
    int brace_count = 0;
    
    for (;;) {
        if (pos >= conf_content.length()) {
            switch (state) {
                case IN_DOUBLE_QUOTE:
                    return CONF_ERROR;
                case IN_SINGLE_QUOTE:
                    return CONF_ERROR;
                case IN_BRACE:
                    return CONF_ERROR;
                default:
                    return CONF_EOF;
            }
        }

        c = conf_content[pos];
        pos++;

        switch (state) {
            case IN_DOUBLE_QUOTE:
                token += c;
                if (c == '\"') {
                    reset_token_state(state, token, tokens);
                }
                break;

            case IN_SINGLE_QUOTE:
                token += c;
                if (c == '\'') {
                    reset_token_state(state, token, tokens);
                }
                break;

            case IN_BRACE:
                token += c;
                if (c == '{') {
                    brace_count++;
                } else if (c == '}') {
                    if (brace_count > 0) {
                        brace_count--;
                    } else {
                        reset_token_state(state, token, tokens);
                        return BLOCK_OK;
                    }
                }
                break;

            default:
                switch (c) {
                    case ';':
                        if (!token.empty()) {
                            tokens.push_back(token);
                            token.clear();
                        }
                        return DIRECTIVE_OK;

                    case '\"':
                    case '\'':
                    case '{':
                        if (!token.empty()) {
                            tokens.push_back(token);
                            token.clear();
                        }
                        token += c;
                        if (c == '\"') state = IN_DOUBLE_QUOTE;
                        else if (c == '\'') state = IN_SINGLE_QUOTE;
                        else state = IN_BRACE;
                        brace_count = 0;
                        break;

                    default:
                        if (isspace(c)) {
                            if (!token.empty()) {
                                tokens.push_back(token);
                                token.clear();
                            }
                        } else {
                            token += c;
                        }
                }
                break;
        }
    }
    
    if (token.empty()) {
        return CONF_EOF;
    }
    return CONF_ERROR;
} 
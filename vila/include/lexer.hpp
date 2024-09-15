//------------------------------------//
// Bản quyền © 2024 Nguyễn Bình Nam
// Được cấp phép theo Giấy phép Apache v2.0
//------------------------------------//

#ifndef LEXER_HPP
#define LEXER_HPP

#pragma once
#include <vector>
#include <string>
#include <any>
#include <map>

#include "token.hpp"
#include "error.hpp"

class Lexer
{
    private:
        // data
        std::string source;
        std::vector<Token> tokens;

        static const std::map<std::string, TokenType> keywords;

        int start = 0;
        int current = 0;
        int line = 1;

        bool is_at_end() const;
        char advance();
        bool match(char expected);
        char peek();
        char peek_next();

        void add_token(TokenType type, std::any literal);
        void add_token(TokenType type);

        void string();
        void number();
        void identifier();
        void scan_token();

    public:
        Lexer(std::string source);
        std::vector<Token> scan_tokens();
};

#endif

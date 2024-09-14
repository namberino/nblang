//------------------------------------//
// Copyright 2024 Nam Nguyen
// Licensed under Apache License v2.0
//------------------------------------//

#include "token.hpp"

Token::Token(TokenType type, std::string lexeme, std::any literal, int line)
    : type(type), lexeme(lexeme), literal(literal), line(line) {}

std::string Token::to_string() const
{
    std::string literal_text;

    switch (type)
    {
        case (IDENTIFIER):
            literal_text = lexeme;
            break;
        case (STRING):
            literal_text = std::any_cast<std::string>(literal);
            break;
        case (NUMBER):
            literal_text = std::to_string(std::any_cast<double>(literal));
            break;
        case (TRUE):
            literal_text = "đúng";
            break;
        case (FALSE):
            literal_text = "sai";
            break;
        default:
            literal_text = "trống";
    }

    return "Loại: " + ::token_to_string(type) + " | Tên: " + lexeme + " | Giá trị: " + literal_text;
} 

std::string token_to_string(TokenType type)
{
    static const std::string strings[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "LEFT_BRACKET", "RIGHT_BRACKET",
        "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR", "PERCENT", "COLON",
        "BANG", "BANG_EQUAL",
        "EQUAL", "EQUAL_EQUAL",
        "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL",
        "STAR_STAR",
        "PLUS_EQUAL", "MINUS_EQUAL", "STAR_EQUAL", "SLASH_EQUAL",
        "IDENTIFIER", "STRING", "NUMBER",
        "AND", "BREAK", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
        "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "MUT", "WHILE", "IMPORT",
        "TOKEN_EOF"
    };

    return strings[static_cast<int>(type)];
}

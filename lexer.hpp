#pragma once
#include "token.hpp"
#include <string>

// Converts a string into a sequence of tokens
class Lexer
{
public:
    Lexer(const std::string& program);

    Token::TokenType peek();
    Token expect(Token::TokenType type);
    bool accept(Token::TokenType type);

private:
    std::string _program;

    Token _token;
    void advance();

    // Current token
    const char* ts;
    const char* te;

    // Machine state
    int act;
    int cs;

    // Buffer state
    const char* p;
    const char* pe;
    const char* eof;
};

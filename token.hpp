#pragma once
#include <iostream>
#include <string>

struct Token
{
    enum TokenType
    {
        Arrow,
        Comma,
        Eof,
        Equals,
        Err,
        Forall,
        Fun,
        Ident,
        In,
        Lbracket,
        Let,
        Lparen,
        Rbracket,
        Rparen,
    };

    TokenType type;
    std::string lexeme;
};

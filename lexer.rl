#include "lexer.hpp"
#include <cstring>
#include <iostream>

// http://willspeak.me/2015/03/24/tokenising-in-c-with-ragel.html

#define CAPTURE(t) \
    _token.type = t; \
    _token.lexeme = std::string(ts, te - ts);

%%{
    machine lexer;

    main := |*
        # Keywords
        'fun' => { CAPTURE(Token::Fun); fbreak; };
        'let' => { CAPTURE(Token::Let); fbreak; };
        'in' => { CAPTURE(Token::In); fbreak; };
        'forall' => { CAPTURE(Token::Forall); fbreak; };

        # Identifiers
        [_a-zA-Z][_a-zA-Z0-9]* => { CAPTURE(Token::Ident); fbreak; };

        # Punctuation / operators
        '(' => { CAPTURE(Token::Lparen); fbreak; };
        ')' => { CAPTURE(Token::Rparen); fbreak; };
        '[' => { CAPTURE(Token::Lbracket); fbreak; };
        ']' => { CAPTURE(Token::Rbracket); fbreak; };
        '=' => { CAPTURE(Token::Equals); fbreak; };
        '->' => { CAPTURE(Token::Arrow); fbreak; };
        ',' => { CAPTURE(Token::Comma); fbreak; };

        # Skip whitespace
        space+;
    *|;
}%%

%% write data;

Lexer::Lexer(const std::string& program)
: _program(program)
{
    %% write init;

    p = _program.c_str();
    pe = p + _program.size();
    eof = pe;

    advance();
}

void Lexer::advance()
{
    if (p < pe)
    {
        %% write exec;

        if (cs == lexer_error)
        {
            throw std::runtime_error("lexer error");
        }
    }
    else
    {
        _token.type = Token::Eof;
    }
}

Token::TokenType Lexer::peek()
{
    return _token.type;
}

Token Lexer::expect(Token::TokenType type)
{
    if (_token.type == type)
    {
        Token current = _token;
        advance();
        return current;
    }
    else
    {
        throw std::runtime_error("syntax error");
    }
}

bool Lexer::accept(Token::TokenType type)
{
    if (_token.type == type)
    {
        advance();
        return true;
    }
    else
    {
        return false;
    }
}

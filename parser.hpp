#pragma once
#include "ast.hpp"
#include "lexer.hpp"

class Parser
{
public:
    Parser(const std::string& program)
    : _lexer(program)
    {}

    ast::Expression* parse();

private:
    ast::Expression* expression();
    ast::Let* letExpression();
    ast::Fun* funExpression();
    ast::Expression* simpleExpression();
    std::vector<ast::Expression*> expressionList();

    Lexer _lexer;
};

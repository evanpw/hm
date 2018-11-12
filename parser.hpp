#pragma once
#include "ast.hpp"
#include "lexer.hpp"

class Parser
{
public:
    Parser(const std::string& program)
    : _lexer(program)
    {}

    ast::Context parse();

private:
    ast::Expr* expression();
    ast::Let* letExpr();
    ast::Fun* funExpr();
    ast::Expr* simpleExpr();
    std::vector<ast::Expr*> expressionList();

    ast::Context _context;
    Lexer _lexer;
};

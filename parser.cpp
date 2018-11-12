#include "parser.hpp"

using namespace ast;

Context Parser::parse()
{
    _context.setRoot(expression());
    return std::move(_context);
}

Expr* Parser::expression()
{
    if (_lexer.peek() == Token::Let)
    {
        return letExpr();
    }
    else if (_lexer.peek() == Token::Fun)
    {
        return funExpr();
    }
    else
    {
        Expr* expr = simpleExpr();

        if (_lexer.accept(Token::Lparen))
        {
            // Function call: f(e1, e2, ...)
            std::vector<Expr*> arguments;
            if (_lexer.peek() != Token::Rparen)
            {
                arguments = expressionList();
            }

            _lexer.expect(Token::Rparen);

            return Call::create(_context, expr, arguments);
        }
        else
        {
            // Just a simple expression
            return expr;
        }
    }
}

// let name = value in body
Let* Parser::letExpr()
{
    _lexer.expect(Token::Let);

    std::string name = _lexer.expect(Token::Ident).lexeme;

    _lexer.expect(Token::Equals);

    Expr* value = expression();

    _lexer.expect(Token::In);

    Expr* body = expression();

    return Let::create(_context, name, value, body);
}

// fun x, y, ... -> body
Fun* Parser::funExpr()
{
    _lexer.expect(Token::Fun);

    // Always at least one parameter
    std::vector<std::string> parameters;
    parameters.push_back(_lexer.expect(Token::Ident).lexeme);

    // And maybe more, separated by commas
    while (_lexer.accept(Token::Comma))
    {
        parameters.push_back(_lexer.expect(Token::Ident).lexeme);
    }

    _lexer.expect(Token::Arrow);

    Expr* body = expression();

    return Fun::create(_context, parameters, body);
}

// identifier or parenthesized expression
Expr* Parser::simpleExpr()
{
    if (_lexer.peek() == Token::Ident)
    {
        std::string name = _lexer.expect(Token::Ident).lexeme;
        return Var::create(_context, name);
    }
    else // parenthesized expression
    {
        _lexer.expect(Token::Lparen);

        Expr* expr = expression();

        _lexer.expect(Token::Rparen);

        return expr;
    }
}

// One or more expression separated by commas
std::vector<Expr*> Parser::expressionList()
{
    std::vector<Expr*> result;

    result.push_back(expression());

    while (_lexer.accept(Token::Comma))
    {
        result.push_back(expression());
    }

    return result;
}

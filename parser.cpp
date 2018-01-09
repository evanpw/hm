#include "parser.hpp"

using namespace ast;

Expression* Parser::parse()
{
    return expression();
}

Expression* Parser::expression()
{
    if (_lexer.peek() == Token::Let)
    {
        return letExpression();
    }
    else if (_lexer.peek() == Token::Fun)
    {
        return funExpression();
    }
    else
    {
        Expression* expr = simpleExpression();

        if (_lexer.accept(Token::Lparen))
        {
            // Function call: f(e1, e2, ...)
            std::vector<Expression*> arguments;
            if (_lexer.peek() != Token::Rparen)
            {
                arguments = expressionList();
            }

            _lexer.expect(Token::Rparen);

            return new Call(expr, arguments);
        }
        else
        {
            // Just a simple expression
            return expr;
        }
    }
}

// let name = value in body
Let* Parser::letExpression()
{
    _lexer.expect(Token::Let);

    std::string name = _lexer.expect(Token::Ident).lexeme;

    _lexer.expect(Token::Equals);

    Expression* value = expression();

    _lexer.expect(Token::In);

    Expression* body = expression();

    return new Let(name, value, body);
}

// fun x, y, ... -> body
Fun* Parser::funExpression()
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

    Expression* body = expression();

    return new Fun(parameters, body);
}

// identifier or parenthesized expression
Expression* Parser::simpleExpression()
{
    if (_lexer.peek() == Token::Ident)
    {
        std::string name = _lexer.expect(Token::Ident).lexeme;
        return new Var(name);
    }
    else // parenthesized expression
    {
        _lexer.expect(Token::Lparen);

        Expression* expr = expression();

        _lexer.expect(Token::Rparen);

        return expr;
    }
}

// One or more expression separated by commas
std::vector<Expression*> Parser::expressionList()
{
    std::vector<Expression*> result;

    result.push_back(expression());

    while (_lexer.accept(Token::Comma))
    {
        result.push_back(expression());
    }

    return result;
}

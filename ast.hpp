#pragma once
#include "visitor.hpp"
#include <string>
#include <vector>

// TODO: Free memory

namespace ast
{

// Generic base class
struct Expression
{
    virtual void accept(Visitor* visitor) = 0;
};

// Variable reference: x
struct Var : public Expression
{
    Var(const std::string& name)
    : name(name)
    {}

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    std::string name;
};

// Function call: f(1, true)
struct Call : public Expression
{
    Call(Expression* function, const std::vector<Expression*>& arguments)
    : function(function), arguments(arguments)
    {}

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    Expression* function;
    std::vector<Expression*> arguments;
};

// Function definition: fn x, y, z -> body
struct Fun : public Expression
{
    Fun(const std::vector<std::string>& parameters, Expression* body)
    : parameters(parameters), body(body)
    {}

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    std::vector<std::string> parameters;
    Expression* body;
};

// Let expression: let name = value in body
struct Let : public Expression
{
    Let(const std::string& name, Expression* value, Expression* body)
    : name(name), value(value), body(body)
    {}

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    std::string name;
    Expression* value;
    Expression* body;
};

} // namespace ast

#pragma once
#include "visitor.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ast
{

class Expr;

// Owner for all AST nodes
class Context
{
public:
    void insert(Expr* expr) { _nodes.emplace_back(expr); }

    Expr* root() { return _root; }
    void setRoot(Expr* root) { _root = root; }

private:
    Expr* _root = nullptr;
    std::vector<std::unique_ptr<Expr>> _nodes;
};

// Generic base class
class Expr
{
public:
    virtual void accept(Visitor* visitor) = 0;
};

// Variable reference: x
class Var : public Expr
{
public:
    static Var* create(Context& context, const std::string& name)
    {
        Var* result = new Var(name);
        context.insert(result);
        return result;
    }

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    std::string name;

private:
    Var(const std::string& name)
    : name(name)
    {}
};

// Function call: f(1, true)
class Call : public Expr
{
public:
    static Call* create(Context& context, Expr* function, const std::vector<Expr*>& arguments)
    {
        Call* result = new Call(function, arguments);
        context.insert(result);
        return result;
    }

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    Expr* function;
    std::vector<Expr*> arguments;

private:
    Call(Expr* function, const std::vector<Expr*>& arguments)
    : function(function), arguments(arguments)
    {}
};

// Function definition: fn x, y, z -> body
class Fun : public Expr
{
public:
    static Fun* create(Context& context, const std::vector<std::string>& parameters, Expr* body)
    {
        Fun* result = new Fun(parameters, body);
        context.insert(result);
        return result;
    }

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    std::vector<std::string> parameters;
    Expr* body;

private:
    Fun(const std::vector<std::string>& parameters, Expr* body)
    : parameters(parameters), body(body)
    {}
};

// Let expression: let name = value in body
struct Let : public Expr
{
public:
    static Let* create(Context& context, const std::string& name, Expr* value, Expr* body)
    {
        Let* result = new Let(name, value, body);
        context.insert(result);
        return result;
    }

    virtual void accept(Visitor* visitor) override { visitor->visit(this); }

    std::string name;
    Expr* value;
    Expr* body;

private:
    Let(const std::string& name, Expr* value, Expr* body)
    : name(name), value(value), body(body)
    {}
};

} // namespace ast

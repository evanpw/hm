#include "semantic.hpp"

using typ::Type;

#define RETURN(x) \
    _type = (x); \
    return

SemanticAnalyzer::SemanticAnalyzer()
{
    // Boostrap with some basic types and values
    Type* Int = new typ::Constant("Int");
    Type* Bool = new typ::Constant("Bool");

    _env.insert("zero", Int);
    _env.insert("one", Int);
    _env.insert("true", Bool);
    _env.insert("false", Bool);

    Type* addType = new typ::Arrow({Int, Int}, Int);
    _env.insert("add", addType);
}

void SemanticAnalyzer::visit(ast::Var* node)
{
    Type* type = _env.lookup(node->name);
    if (!type)
    {
        throw std::runtime_error("undefined variable: " + node->name);
    }

    RETURN(instantiate(type, _level));
}

void SemanticAnalyzer::visit(ast::Call* node)
{
    Type* fnType = infer(node->function);

    std::vector<Type*> argTypes;
    for (auto* arg : node->arguments)
    {
        argTypes.push_back(infer(arg));
    }

    // Solve for the return type of the function call
    Type* outType = typ::Var::makeUnbound(_level);
    Type* expectedType = new typ::Arrow(argTypes, outType);
    if (!unify(fnType, expectedType))
    {
        throw std::runtime_error("unification error");
    }

    RETURN(outType);
}

void SemanticAnalyzer::visit(ast::Fun* node)
{
    // Function definitions define a new scope
    _env.enterScope();

    // Function parameters start out arbitrary, are constrained by
    // their usage in the function body
    std::vector<Type*> paramTypes;
    for (auto& param : node->parameters)
    {
        Type* paramType = typ::Var::makeUnbound(_level);
        paramTypes.push_back(paramType);
        _env.insert(param, paramType);
    }

    Type* bodyType = infer(node->body);

    _env.exitScope();

    RETURN(new typ::Arrow(paramTypes, bodyType));
}

void SemanticAnalyzer::visit(ast::Let* node)
{
    // Keep track of the level of let-nesting in order to optimize generalization
    _level += 1;
    Type* valueType = infer(node->value);
    _level -= 1;

    // Let-generalization
    Type* genValueType = generalize(valueType, _level);

    // The body of a let statement defines a new scope
    _env.enterScope();
    _env.insert(node->name, genValueType);
    Type* bodyType = infer(node->body);
    _env.exitScope();

    RETURN(bodyType);
}

#pragma once
#include "ast.hpp"
#include "type_env.hpp"

class SemanticAnalyzer : public ast::Visitor
{
public:
    SemanticAnalyzer();

    typ::Type* infer(ast::Expression* node)
    {
        node->accept(this);
        return _type;
    }

    void visit(ast::Var* node) override;
    void visit(ast::Call* node) override;
    void visit(ast::Fun* node) override;
    void visit(ast::Let* node) override;

private:
    // For most recently-checked AST node
    typ::Type* _type;

    int _level = 0;
    typ::TypeEnvironment _env;
};

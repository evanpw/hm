#pragma once
#include "visitor.hpp"

namespace ast
{

struct Var;
struct Call;
struct Fun;
struct Let;

struct Visitor
{
    virtual void visit(Var* node) = 0;
    virtual void visit(Call* node) = 0;
    virtual void visit(Fun* node) = 0;
    virtual void visit(Let* node) = 0;
};

} // namespace ast

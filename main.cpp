#include "parser.hpp"
#include "semantic.hpp"
#include <cassert>
#include <iostream>

int main()
{
    Parser parser("add(one, one)");
    ast::Expression* expr = parser.parse();

    SemanticAnalyzer semant;
    typ::Type* type = semant.infer(expr);
    std::cout << type << "\n";

    return 0;
}

#include "parser.hpp"
#include "semantic.hpp"
#include <cassert>
#include <iostream>

int main()
{
    Parser parser("add(one, one)");
    ast::Context ast = parser.parse();

    SemanticAnalyzer semant;
    typ::Type* type = semant.infer(ast.root());
    std::cout << type << "\n";

    return 0;
}

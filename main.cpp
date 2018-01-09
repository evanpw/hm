#include "parser.hpp"
#include "semantic.hpp"
#include <cassert>
#include <iostream>

int main()
{
    /*
    let programs = vec![
        "let f = fun x, y -> x in f(zero, one)",
        //"add(zero, true)",
        "fun x -> x",
        "let f = fun x -> x in f",
        "pair(one, true)",
        "let f = fun x -> x in pair(f(one), f(true))",
        //"fun x -> let y = x in y(y)",
        "fun x -> let y = x in y",
        "fun x -> let y = fun z -> z in y",
        "fun x -> let y = fun z -> x(z) in y",
        "add(one, one, one)",
    ];
    */

    Parser parser("add(one, one, one)");
    ast::Expression* expr = parser.parse();

    SemanticAnalyzer semant;
    typ::Type* type = semant.infer(expr);
    std::cout << type << "\n";

    return 0;
}

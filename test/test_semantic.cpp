#include "parser.hpp"
#include "semantic.hpp"
#include <gtest/gtest.h>
#include <iostream>

std::string inferType(const std::string& program)
{
    Parser parser(program);
    ast::Context ast = parser.parse();

    SemanticAnalyzer semant;
    typ::Type* type = semant.infer(ast.root());

    std::stringstream ss;
    ss << type;
    return ss.str();
}

TEST(SemanticTest, Inference)
{
    EXPECT_EQ(inferType("let f = fun x, y -> x in f(zero, one)"), "Int");
    EXPECT_EQ(inferType("fun x -> x"), "a -> a");
    EXPECT_EQ(inferType("let f = fun x -> x in f"), "a -> a");
    EXPECT_EQ(inferType("fun x -> let y = fun z -> z in y"), "a -> (b -> b)");
    EXPECT_EQ(inferType("fun x -> let y = fun z -> x(z) in y"), "|a -> b| -> (a -> b)");

    EXPECT_EQ(inferType("let f = fun x -> x in f(one)"), "Int");
    EXPECT_EQ(inferType("nonzero(one)"), "Bool");
    EXPECT_EQ(inferType("let f = nonzero in f(one)"), "Bool");
    EXPECT_EQ(inferType("let f = fun x -> nonzero(x) in f(one)"), "Bool");
    EXPECT_EQ(inferType("(fun x -> nonzero(x))(one)"), "Bool");
    EXPECT_EQ(inferType("let x = one in nonzero(x)"), "Bool");

    EXPECT_EQ(inferType("fun x -> fun y -> x"), "a -> (b -> a)");
    EXPECT_EQ(inferType("(fun x -> fun y -> x)(one)"), "a -> Int");
    EXPECT_EQ(inferType("let f = fun x -> fun y -> y in f(one)"), "a -> a");
    EXPECT_EQ(inferType("fun x, y -> x"), "|a, b| -> a");
    EXPECT_EQ(inferType("fun f -> eq(f(one), one)"), "|Int -> Int| -> Bool");
}

// These tests would fail with naive generalization
TEST(SemanticTest, LevelGeneralization)
{
    EXPECT_EQ(inferType("fun x -> let y = x in y"), "a -> a");
    EXPECT_EQ(inferType("fun x -> let y = fun z -> x in y"), "a -> (b -> a)");
}

TEST(SemanticTest, TypeErrors)
{
    // Wrong argument types
    EXPECT_THROW(inferType("add(zero, true)"), std::runtime_error);

    // Infinite recursive type
    EXPECT_THROW(inferType("fun x -> let y = x in y(y)"), std::runtime_error);

    // Wrong function arity
    EXPECT_THROW(inferType("add(one, one, one)"), std::runtime_error);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

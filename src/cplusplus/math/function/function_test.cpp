#include "gtest/gtest.h"
#include "Gold/math/function.hpp"
#include <iostream>
#include <exception>

using namespace Gold::math;

TEST(Constructor, LLValueConstructors) {
    variable var("x");
    expression expr("x+2");
    EXPECT_NO_THROW(function(var, expr));    
}

TEST(Constructor, RLValueConstructors) {
    variable var("x");
    expression expr("x+2");
    EXPECT_NO_THROW(function(std::move(var), expr));    
}

TEST(Constructor, LRValueConstructors) {
    variable var("x");
    expression expr("x+2");
    EXPECT_NO_THROW(function(var, std::move(expr)));    
}

TEST(Constructor, RRValueConstructors) {
    variable var("x");
    expression expr("x+2");
    EXPECT_NO_THROW(function(std::move(var), std::move(expr)));    
}

TEST(Operator, Expressions) {
    variable x("x");
    variable y("y");
    function f({x, y}, x+y);
    expression result = f({x,y});
    EXPECT_EQ("x+y", result.string());
}

TEST(Evaluate, Numbers) {
    variable x("x");
    variable y("y");
    function f({x, y}, x+y);
    double result = f.evaluate({5,10});
    EXPECT_EQ(15, result);
}

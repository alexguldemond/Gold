#include "gtest/gtest.h"
#include "Gold/math/expression.hpp"
#include <iostream>
#include <exception>

using namespace Gold::math;

TEST(Expression, Constructors) {
    expression expr("2+(-1)");
    EXPECT_TRUE(expr.defined());
    EXPECT_EQ("2+-1", expr.string());

    expression expr2 = expr;
    EXPECT_TRUE(expr2.defined());
    EXPECT_EQ("2+-1", expr2.string());

    expression expr3 (std::move(expr2));
    EXPECT_TRUE(expr3.defined());
    EXPECT_EQ("2+-1", expr3.string());
    EXPECT_FALSE(expr2.defined());
}

TEST(Evaluation, Sucess) {
    expression expr("a+b^2");
    EXPECT_EQ(5, expr( {{ "a", 1}, {"b", 2}}) );
    EXPECT_EQ(5, expr( {{ "a", 1}, {"b", -2}}) );
    
    expr = expression("a/(b*c)");
    EXPECT_EQ( 0, expr({{"a", 0}, {"b", 10}, {"c", 20}} ));
    EXPECT_EQ(2, expr({{"a", 20}, {"b", 2}, {"c", 5}}));
    
    expr = expression("a/(b*-c)");
    EXPECT_EQ( 0, expr({{"a", 0}, {"b", 10}, {"c", 20}} ));
    EXPECT_EQ(-2, expr({{"a", 20}, {"b", 2}, {"c", 5}}));
}

TEST(Addition, TwoAddNodes) {
    expression a("a+b");
    expression b("a+c");
    expression c(a+b);
    EXPECT_EQ("a+b+a+c", c.string());
}

TEST(Addition, OneAddNode) {
    expression a("a*b");
    expression b("a+b");
    expression c(a+b);
    EXPECT_EQ("a*b+a+b", c.string());
}

TEST(Addition, NoAddNodes) {
    expression a("a*b");
    expression b("b*a");
    expression c(a+b);
    EXPECT_EQ("a*b+b*a", c.string());
}

TEST(Negation, AddNode) {
    expression a("a+b");
    expression b = -a;
    EXPECT_EQ("-1*(a+b)", b.string());
}

TEST(Negation, MultiplyNode) {
    expression a("a*b");
    expression b = -a;
    EXPECT_EQ("-1*a*b", b.string());
}


TEST(Negation, DivideNode) {
    expression a("a/b");
    expression b = -a;
    EXPECT_EQ("-1*a*b^(-1)", b.string());
}

TEST(Negation, Number) {
    expression a(5);
    expression b = -a;
    EXPECT_EQ("-1*5", b.string());
}

TEST(Subtraction, AddNode) {
    expression a("a+b");
    expression b("a*b");
    EXPECT_EQ("a+b+-1*a*b", (a-b).string());
}

TEST(Subtraction, FuncNode) {
    expression a("a+b");
    expression b("Func[a,b]");
    EXPECT_EQ("a+b+-1*Func[a, b]", (a-b).string());
}

TEST(Multiplication, MultNode) {
    expression a("a*b");
    expression b("c*d");
    EXPECT_EQ("a*b*c*d", (a*b).string());
}

TEST(Multiplication, LeftMultNode) {
    expression a("a*b");
    expression b("c+d");
    EXPECT_EQ("a*b*(c+d)", (a*b).string());
}

TEST(Multiplication, RightMultNode) {
    expression a("a+b");
    expression b("c*d");
    expression c = a*b;
    EXPECT_EQ("(a+b)*c*d", (a*b).string());
}

TEST(Division, Adds) {
    expression a("a+b");
    expression b("c+d");
    EXPECT_EQ("(a+b)*(c+d)^(-1)", (a/b).string());
}

TEST(Division, Mults) {
    expression a("a*b");
    expression b("c*d");
    EXPECT_EQ("a*b*(c*d)^(-1)", (a/b).string());
}

TEST(Division, Power) {
    expression a("a^b");
    expression b("c^d");
    EXPECT_EQ("a^b*(c^d)^(-1)", (a/b).string());
}

TEST(Division, Function) {
    expression a("Func[a^b]");
    expression b("Gunc[c^d]");
    EXPECT_EQ("Func[a^b]*Gunc[c^d]^(-1)", (a/b).string());
}

TEST(PowerQ, Adds) {
    expression a("a+b");
    expression b("c+d");
    EXPECT_EQ("(a+b)^(c+d)", pow(a,b).string());
}

TEST(Power, Mults) {
    expression a("a*b");
    expression b("c*d");
    EXPECT_EQ("(a*b)^(c*d)", pow(a,b).string());
}

TEST(Power, Power) {
    expression a("a^b");
    expression b("c^d");
    EXPECT_EQ("(a^b)^c^d", pow(a,b).string());
}

TEST(Power, Function) {
    expression a("Func[a^b]");
    expression b("Gunc[c^d]");
    EXPECT_EQ("Func[a^b]^Gunc[c^d]", pow(a,b).string());
}

TEST(Derivative, Add) {
    expression a("x+y");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Subtract) {
    expression a("y-x");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("-1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Multiply) {
    expression a("x*y");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("y", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, DivideOne) {
    expression a("x/y");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("y^(-1)", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, DivideTwo) {
    expression a("y/x");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("y*x^(-1+-1)*(-1)*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Exponent) {
    expression a("x^2");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("x^(2+-1)*2*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated2) {
    expression a("3*x^4");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("3*x^(4+-1)*4*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated3) {
    expression a("x^2+3*x^4");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("x^(2+-1)*2*1+3*x^(4+-1)*4*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated4) {
    expression a("5/(x^2)");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("5*(x^2)^(-1+-1)*(-1)*x^(2+-1)*2*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated5) {
    expression a("4^x");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("4^x*Ln[4]*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated6) {
    expression a("5/(x^2)+4^x");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("5*(x^2)^(-1+-1)*(-1)*x^(2+-1)*2*1+4^x*Ln[4]*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}


TEST(Derivative, Complicated8) {
    expression a("(x^2+1)");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("x^(2+-1)*2*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated9) {
    expression a("x^(-1)");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("x^(-1+-1)*(-1)*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated9_2) {
    expression a("1/x");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("x^(-1+-1)*(-1)*1", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated10) {
    expression a("(x^2+1)^(x^(-1))");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("(x^2+1)^x^(-1)*(x^(-1)*x^(2+-1)*2*1*(x^2+1)^(-1)+Ln[x^2+1]*x^(-1+-1)*(-1)*1)", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Complicated10_2) {
    expression a("(x^2+1)^(1/x)");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("(x^2+1)^x^(-1)*(x^(-1)*x^(2+-1)*2*1*(x^2+1)^(-1)+Ln[x^2+1]*x^(-1+-1)*(-1)*1)", string);
    }
    catch (std::string e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, Function) {
    expression a("Sin[x]");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("Cos[x]", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}

TEST(Derivative, SomeFunctions) {
    expression a("Sin[Exp[x]]");
    try {
	std::string string = derivative(a, "x").string();
	EXPECT_EQ("Cos[Exp[x]]*Exp[x]", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}

TEST(ChangeVariables, Add) {
    expression a("a+b");
    try {
	expression result = a( {{"a", expression("a+1")}, {"b", expression("c^2")}} );
	std::string string = result.string();
	EXPECT_EQ("a+1+c^2", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}

TEST(ChangeVariables, Multiply) {
    expression a("a*b");
    try {
	expression result = a( {{"a", expression("2*a")}, {"b", expression("c+2")}} );
	std::string string = result.string();
	EXPECT_EQ("2*a*(c+2)", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}

TEST(ChangeVariables, Power) {
    expression a("a^b");
    try {
	expression result = a( {{"a", expression("2*a")}, {"b", expression("c+2")}} );
	std::string string = result.string();
	EXPECT_EQ("(2*a)^(c+2)", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}

TEST(ChangeVariables, Function) {
    expression a("Func[a,b]");
    try {
	expression result = a( {{"a", expression("2*a")}, {"b", expression("c+2")}, {"Func", expression("Barf")}} );
	std::string string = result.string();
	EXPECT_EQ("Func[2*a, c+2]", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}

TEST(ChangeVariables, Variable) {
    expression a("a");
    try {
	expression result = a( {{"a", expression("2*a")}, {"b", expression("c+2")}, {"Func", expression("Barf")}} );
	std::string string = result.string();
	EXPECT_EQ("2*a", string);
    }
    catch (std::string& e) {
	EXPECT_EQ(e, "");
    }
}
	
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


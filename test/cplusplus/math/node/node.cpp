#include "gtest/gtest.h"
#include "Gold/math/node.hpp"

using namespace Gold::math::node;

TEST(Node, Constructors) {
    base_node::ptr root = std::make_unique<Gold::math::node::add>();
    EXPECT_EQ("+", root->get_token());

    root = std::make_unique<Gold::math::node::multiply>();
    EXPECT_EQ("*", root->get_token());

    root = std::make_unique<Gold::math::node::power>();
    EXPECT_EQ("^", root->get_token());

    root = std::make_unique<Gold::math::node::function>("Sin");
    EXPECT_EQ("Sin", root->get_token());

    root = std::make_unique<Gold::math::node::integer>(5);
    EXPECT_EQ("5", root->get_token());

    root = std::make_unique<Gold::math::node::number>(5);
    EXPECT_EQ("5.000000", root->get_token());

    root = std::make_unique<Gold::math::node::number>(5.1);
    EXPECT_EQ("5.100000", root->get_token());

    root = std::make_unique<Gold::math::node::number>("5.1");
    EXPECT_EQ("5.100000", root->get_token());

    root = std::make_unique<Gold::math::node::variable>("x");
    EXPECT_EQ("x", root->get_token());

    integer five(5);
    integer ten(10);

    root = inverse::make_inverse(five);
    EXPECT_EQ("^", root->get_token());
    EXPECT_EQ("1", root->numerator()->get_token());
    EXPECT_EQ("5", root->denominator()->get_token());

    root = std::make_unique<quotient>(five, ten);
    EXPECT_EQ("*", root->get_token());
    EXPECT_EQ("5", root->numerator()->get_token());
    EXPECT_EQ("10", root->denominator()->get_token());

    root = std::make_unique<rational>(five, ten);
    EXPECT_EQ("*", root->get_token());
    EXPECT_EQ("5", root->numerator()->get_token());
    EXPECT_EQ("10", root->denominator()->get_token());
}

TEST(Evaluatation, Integer) {
    base_node::ptr root = std::make_unique<integer>(5);
    ASSERT_EQ(5, root->evaluate());
}

TEST(Evaluation, Double) {
    base_node::ptr root = std::make_unique<number>(5.1);
    ASSERT_EQ(5.1, root->evaluate());
}

TEST(Evaluation, Variable) {
    base_node::ptr root = std::make_unique<variable>("x");
    
    std::map<std::string, double> vars {{ "x", 5 }};
    ASSERT_EQ(5, root->evaluate(vars));
}

TEST(Evaluation, Addition) {
    base_node::vec vector;
    for (int i = 0; i < 5; i++) {
	vector.push_back(std::make_unique<integer>(i));
    }
    base_node::ptr root = std::make_unique<add>(vector);
    EXPECT_EQ(10, root->evaluate());
}
	
TEST(Evaluation, Multiplication) {
    base_node::vec vector;
    for (int i = 2; i < 5; i++) {
	vector.push_back(std::make_unique<integer>(i));
    }
    base_node::ptr root = std::make_unique<multiply>(vector);
    EXPECT_EQ(24, root->evaluate());
}

TEST(Evaluation, Exponentiation) {
    base_node::ptr root = std::make_unique<power>(integer(2), integer(3));
    EXPECT_EQ(8, root->evaluate());
}

TEST(Evaluation, Inverse) {
    base_node::ptr x = std::make_unique<integer>(5);
    base_node::ptr root = inverse::make_inverse(*x);
    EXPECT_EQ(.2, root->evaluate());
}

TEST(Evaluation, Quotient) {
    base_node::ptr root = std::make_unique<quotient>(integer(5),integer(10));
    EXPECT_EQ(.5, root->evaluate());
}

TEST(Evaluation, Rational) {
    base_node::ptr root = std::make_unique<rational>(integer(5),integer(10));
    EXPECT_EQ(.5, root->evaluate());
}

TEST(Evaluation, Function) {
    variable::ptr x = std::make_unique<variable>("x");
    base_node::vec x_vec;
    x_vec.push_back(std::move(x));
    base_node::ptr root = std::make_unique<function>("Abs", x_vec);
    EXPECT_EQ(5, root->evaluate( {{"x", 5}}));
    EXPECT_EQ(5, root->evaluate( {{"x", -5}}));
}


TEST(MakeLeafNode, Test) {
    base_node::ptr root = make_leaf_node("1");
    EXPECT_EQ(root->get_token(), "1");
    
    root = make_leaf_node("1.5");
    EXPECT_EQ(root->get_token(), "1.500000");

    root = make_leaf_node("Hello");
    EXPECT_EQ(root->get_token(), "Hello");

    root = make_leaf_node("-5");
    EXPECT_EQ(root->get_token(), "-5");
}

TEST(MakeFunctionTest, Test) {
    base_node::ptr root = make_function_node("Sin[10]");
    EXPECT_EQ(root->get_token(), "Sin");
    ASSERT_EQ(root->children.size(), uint(1));
    EXPECT_EQ(root->children[0]->get_token(), "10");

    root = make_function_node("Sin[x,y,z]");
    EXPECT_EQ(root->get_token(), "Sin");
    ASSERT_EQ(root->children.size(), uint(3));
    EXPECT_EQ(root->children[0]->get_token(), "x");
    EXPECT_EQ(root->children[1]->get_token(), "y");
    EXPECT_EQ(root->children[2]->get_token(), "z");
}

TEST(MakeTree, LeafTest) {
    base_node::ptr root = make_tree("1");
    EXPECT_EQ(root->get_token(), "1");
    
    root = make_tree("1.5");
    EXPECT_EQ(root->get_token(), "1.500000");

    root = make_tree("Hello");
    EXPECT_EQ(root->get_token(), "Hello");

    root = make_leaf_node("-5");
    EXPECT_EQ(root->get_token(), "-5");
}

TEST(MakeTree, FunctionTest) {
    base_node::ptr root = make_tree("Sin[10]");
    EXPECT_EQ(root->get_token(), "Sin");
    ASSERT_EQ(root->children.size(), uint(1));
    EXPECT_EQ(root->children[0]->get_token(), "10");

    root = make_tree("Sin[x,y,z]");
    EXPECT_EQ(root->get_token(), "Sin");
    ASSERT_EQ(root->children.size(), uint(3));
    EXPECT_EQ(root->children[0]->get_token(), "x");
    EXPECT_EQ(root->children[1]->get_token(), "y");
    EXPECT_EQ(root->children[2]->get_token(), "z");
}

TEST(MakeTree, GeneralTest) {
    base_node::ptr root = make_tree("a+b");
    EXPECT_EQ(root->get_token(), "+");
    ASSERT_EQ(root->children.size(), uint(2));
    EXPECT_EQ(root->children[0]->get_token(), "a");
    EXPECT_EQ(root->children[1]->get_token(), "b");

    root = make_tree("a-b");
    EXPECT_EQ(root->get_token(), "+");
    ASSERT_EQ(root->children.size(), uint(2));
    EXPECT_EQ(root->children[0]->get_token(), "a");
    EXPECT_EQ(root->children[1]->children.size(), uint(2));
    EXPECT_EQ(root->children[1]->children[0]->get_token(), "-1");
    EXPECT_EQ(root->children[1]->children[1]->get_token(), "b");
}

TEST(MakeString, Test) {
    base_node::ptr root = make_tree("a+b+c+d");
    EXPECT_EQ("a+b+c+d", root->string());

    root = make_tree("a-b*(c/d)");
    EXPECT_EQ("a+-1*b*c*d^(-1)", root->string());

    root = make_tree("Func[a,b,a*b]");
    EXPECT_EQ("Func[a, b, a*b]", root->string());

    root = make_tree("2^(a/b)");
    EXPECT_EQ("2^(a*b^(-1))", root->string());

    root = make_tree("-Func[a]");
    EXPECT_EQ("-1*Func[a]", root->string());
}

TEST(MakeString, Inverse) {
    base_node::ptr root = make_tree("1/x");
    EXPECT_EQ("x^(-1)", root->string());
}

TEST(MakeString, Quotient) {
    base_node::ptr root = make_tree("y/x");
    EXPECT_EQ("y*x^(-1)", root->string());
}

TEST(Is, Add) {
    base_node::ptr root = make_tree("a+b");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
}

TEST(Is, Multiply) {
    base_node::ptr root = make_tree("a*b");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
    
    root = make_tree("a*b*0");
    EXPECT_TRUE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("1*1*1*(-1)*(-1)");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("1*1*(-1)*(-1)*(-1)");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());
}

TEST(Is, Power) {
    base_node::ptr root = make_tree("a^b");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("0^b");
    EXPECT_TRUE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("a^0");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("1^b");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("(-1)^1");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());
}

TEST(Is, Function) {
    auto root = make_tree("F[a,b]");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
}

TEST(Is, Integer) {
    auto root = make_tree("3");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("0");
    EXPECT_TRUE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("1");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
    
    root = make_tree("-1");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());
}

TEST(Is, Number) {
    auto root = make_tree("3.0");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("0.0");
    EXPECT_TRUE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("1.0");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
    
    root = make_tree("-1.0");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());
}

TEST(Is, Variable) {
    auto root = make_tree("a");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
}

TEST(Is, Inverse) {
    auto root = inverse::make_inverse(*(make_tree("a")));
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = inverse::make_inverse(*(make_tree("1")));
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = inverse::make_inverse(*(make_tree("-1")));
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());

    root = inverse::make_inverse(*(make_tree("0")));
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
}

TEST(Is, Quotient) {
    auto root = make_tree("a/b");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("1/1");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());

    root = make_tree("(-1)/1");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());
    
    root = make_tree("1/(-1)");
    EXPECT_FALSE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_TRUE(root->is_minus_one());
   
    root = make_tree("(-1)/(-1)");
    EXPECT_FALSE(root->is_zero());
    EXPECT_TRUE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
    
    root = make_tree("0/b");
    EXPECT_TRUE(root->is_zero());
    EXPECT_FALSE(root->is_one());
    EXPECT_FALSE(root->is_minus_one());
}

TEST(IsUndefined, FalseCases) {
    auto root = make_tree("a+b/(c^2)");
    EXPECT_FALSE(root->is_undefined());
    
    root = make_tree("a+b/(c^d)+0/1");
    EXPECT_FALSE(root->is_undefined());
}

TEST(IsUndefined, TrueCases) {
    auto root = make_tree("0/0");
    EXPECT_TRUE(root->is_undefined());

    root = make_tree("a/0");
    EXPECT_TRUE(root->is_undefined());

    root = make_tree("0^0");
    EXPECT_TRUE(root->is_undefined());
    
    root = make_tree("a+b/(c^2)+0/0");
    EXPECT_TRUE(root->is_undefined());
    
    root = make_tree("a+b/(c^d)+1/0");
    EXPECT_TRUE(root->is_undefined());

    root = make_tree("a+0^0");
    EXPECT_TRUE(root->is_undefined());
}

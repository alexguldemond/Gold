#include "gtest/gtest.h"
#include "Gold/math/node/node.hpp"

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
    ASSERT_EQ(5, (*root)());
}

TEST(Evaluation, Double) {
    base_node::ptr root = std::make_unique<number>(5.1);
    ASSERT_EQ(5.1, (*root)());
}

TEST(Evaluation, Variable) {
    base_node::ptr root = std::make_unique<variable>("x");
    
    std::map<std::string, double> vars {{ "x", 5 }};
    ASSERT_EQ(5, (*root)(vars));
}

TEST(Evaluation, Addition) {
    base_node::vec vector;
    for (int i = 0; i < 5; i++) {
	vector.push_back(std::make_unique<integer>(i));
    }
    base_node::ptr root = std::make_unique<add>(vector);
    EXPECT_EQ(10, (*root)());
}
	
TEST(Evaluation, Multiplication) {
    base_node::vec vector;
    for (int i = 2; i < 5; i++) {
	vector.push_back(std::make_unique<integer>(i));
    }
    base_node::ptr root = std::make_unique<multiply>(vector);
    EXPECT_EQ(24, (*root)());
}

TEST(Evaluation, Exponentiation) {
    base_node::ptr root = std::make_unique<power>(integer(2), integer(3));
    EXPECT_EQ(8, (*root)());
}

TEST(Evaluation, Inverse) {
    base_node::ptr x = std::make_unique<integer>(5);
    base_node::ptr root = inverse::make_inverse(*x);
    EXPECT_EQ(.2, (*root)());
}

TEST(Evaluation, Quotient) {
    base_node::ptr root = std::make_unique<quotient>(integer(5),integer(10));
    EXPECT_EQ(.5, (*root)());
}

TEST(Evaluation, Rational) {
    base_node::ptr root = std::make_unique<rational>(integer(5),integer(10));
    EXPECT_EQ(.5, (*root)());
}

TEST(Evaluation, Function) {
    variable::ptr x = std::make_unique<variable>("x");
    base_node::vec x_vec;
    x_vec.push_back(std::move(x));
    base_node::ptr root = std::make_unique<function>("Abs", x_vec);
    EXPECT_EQ(5, (*root)( {{"x", 5}}));
    EXPECT_EQ(5, (*root)( {{"x", -5}}));
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

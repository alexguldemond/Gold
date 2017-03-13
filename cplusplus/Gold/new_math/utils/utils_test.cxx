#include "gtest/gtest.h"
#include "Gold/new_math/utils/utils.hpp"

using namespace Gold::math::utils;

TEST(GreatestCommonDivisorTest, RelativelyPrimeNos) {
    EXPECT_EQ(1, greatest_common_divisor(3,5));
    EXPECT_EQ(1, greatest_common_divisor(8,5));
    EXPECT_EQ(1, greatest_common_divisor(11,20));
}

TEST(GreatestCommonDivisorTest, NotRelativelyPrimeNos) {
    EXPECT_EQ(5, greatest_common_divisor(5,5));
    EXPECT_EQ(5, greatest_common_divisor(15,20));
    EXPECT_EQ(20, greatest_common_divisor(40,60));
}

TEST(IsCharNum, NumberChars) {
    EXPECT_TRUE( is_char_num('4'));
    EXPECT_TRUE( is_char_num('5'));
}

TEST(IsCharNum, NonNumberChars) {
    EXPECT_FALSE( is_char_num('A'));
    EXPECT_FALSE( is_char_num('+'));
}

TEST(IsStringInteger, IntegerString) {
    EXPECT_TRUE( is_string_integer("1234"));
    EXPECT_TRUE( is_string_integer("-533"));
}

TEST(IsStringInteger, NonIntegerString) {
    EXPECT_FALSE( is_string_integer("Alex Guldemond"));
    EXPECT_FALSE( is_string_integer("533.1"));
    EXPECT_FALSE( is_string_integer("1+2"));
}

TEST(IsStringNumber, NumberString) {
    EXPECT_TRUE( is_string_num("12.43"));
    EXPECT_TRUE( is_string_num("-0.435"));
    EXPECT_TRUE( is_string_num("-.43"));
}

TEST(IsStringNumber, NonNumberString) {
    EXPECT_FALSE( is_string_num("Alex Guldemond"));
    EXPECT_FALSE( is_string_num("-0.4a"));
    EXPECT_FALSE( is_string_num("1+43"));
}

TEST(HasUncontainedOp, TrueCase) {
    EXPECT_TRUE(has_uncontained_op("a+b", '+'));
    EXPECT_TRUE(has_uncontained_op("a-b", '-'));
    EXPECT_TRUE(has_uncontained_op("a*b", '*'));
    EXPECT_TRUE(has_uncontained_op("a/b", '/'));
    EXPECT_TRUE(has_uncontained_op("a^b", '^'));
    EXPECT_TRUE(has_uncontained_op("Sin[a+b]+c", '+'));
}

TEST(HasUncontainedOp, FalseCase) {
    EXPECT_FALSE(has_uncontained_op("(a+b)", '+'));
    EXPECT_FALSE(has_uncontained_op("(a-b)", '-'));
    EXPECT_FALSE(has_uncontained_op("(a*b)", '*'));
    EXPECT_FALSE(has_uncontained_op("(a/b)", '/'));
    EXPECT_FALSE(has_uncontained_op("Sin[a+b]", '+'));
}

TEST(AreParensMatched, TrueCase) {
    EXPECT_TRUE(are_parens_matched("Sin[x^2]+4*((a*b)/(c+d))"));
    EXPECT_TRUE(are_parens_matched("a"));
}


TEST(AreParensMatched, FalseCase) {
    EXPECT_FALSE(are_parens_matched("(a"));
    EXPECT_FALSE(are_parens_matched("Sin[(a+b]+c)"));
}

TEST(IsFunction, TrueCase) {
    EXPECT_TRUE(is_function("HelloMoFo[x^2+b]"));
    EXPECT_TRUE(is_function("HelloMoFo[a]"));
}

TEST(IsFunction, FalseCase) {
    EXPECT_FALSE(is_function("HelloMoFo[x^2+b]+1"));
    EXPECT_FALSE(is_function("HelloMoFo[a]^2"));
}

TEST(NeedsParens, TrueCase) {
    EXPECT_TRUE(needs_parens("a+b", "*", true));
    EXPECT_TRUE(needs_parens("Sin[x]+Cos[y]","/", false));
}

TEST(NeedsParens, FalseCase) {
    EXPECT_FALSE(needs_parens("a+b", "+", true));
    EXPECT_FALSE(needs_parens("Sin[x]","/", false));
}

TEST(Contains, Indexes) {
    std::vector<std::string> test_vec {"a", "b", "c", "d"}; 
    EXPECT_EQ(0, contains("a", test_vec));
    EXPECT_EQ(1, contains("b", test_vec));
    EXPECT_EQ(2, contains("c", test_vec));
    EXPECT_EQ(3, contains("d", test_vec));
    EXPECT_EQ(-1, contains("e", test_vec));
}

TEST(FindLowestPriority, Expressions) {
    ASSERT_EQ(1, find_lowest_priority("a+b"));
    ASSERT_EQ(5, find_lowest_priority("(a-b)*(d^c)"));
    ASSERT_EQ(1, find_lowest_priority("a^b^c"));
    ASSERT_EQ(9, find_lowest_priority("a*(x+y+x)+3"));
    ASSERT_EQ(-1, find_lowest_priority("a"));
    ASSERT_EQ(-2, find_lowest_priority("Sin[a*b/(c+d)]"));
    ASSERT_EQ(8, find_lowest_priority("Cos[x-y]+c"));
}

TEST(AppendWithParens, Addition) {
    std::string test_string = "a+b";
    bool rhs = true;
    std::string op = "+";
    append_with_parens(test_string, "a+b", op, rhs);
    EXPECT_EQ("a+b+a+b", test_string);

    test_string = "a*b";
    append_with_parens(test_string, "a*b", op, rhs);
    EXPECT_EQ("a*b+a*b", test_string);
}

TEST(AppendWithParens, Subtraction) {
    std::string test_string = "a+b";
    bool rhs = true;
    std::string op = "-";
    append_with_parens(test_string, "a+b", op, rhs);
    EXPECT_EQ("a+b-(a+b)", test_string);

    test_string = "a*b";
    append_with_parens(test_string, "a", op, rhs);
    EXPECT_EQ("a*b-a", test_string);

    test_string = "a*b";
    append_with_parens(test_string, "a*b", op, rhs);
    EXPECT_EQ("a*b-a*b", test_string);
}

TEST(AppendWithParens, Multiplication) {
    std::string test_string = "a+b";
    bool rhs = true;
    std::string op = "*";
    append_with_parens(test_string, "a+b", op, rhs);
    EXPECT_EQ("a+b*(a+b)", test_string);

    test_string = "a+b";
    append_with_parens(test_string, "a+b", op, rhs);
    EXPECT_EQ("a+b*(a+b)", test_string);
}

TEST(AppendWithParens, Division) {
    std::string test_string = "a+b";
    bool rhs = true;
    std::string op = "/";
    append_with_parens(test_string, "a+b", op, rhs);
    EXPECT_EQ("a+b/(a+b)", test_string);

    test_string = "a+b";
    append_with_parens(test_string, "a*b", op, rhs);
    EXPECT_EQ("a+b/(a*b)", test_string);
}

TEST(AppendWithParens, Exponentiation) {
    std::string test_string = "a+b";
    bool rhs = true;
    std::string op = "^";
    append_with_parens(test_string, "a+b", op, rhs);
    EXPECT_EQ("a+b^(a+b)", test_string);

    test_string = "a+b";
    append_with_parens(test_string, "a*b", op, rhs);
    EXPECT_EQ("a+b^(a*b)", test_string);
}

TEST(ReplaceAll, Test) {
    EXPECT_EQ("Hello Friend", replace_all("Hxllo Frixnd", "x", "e"));
    EXPECT_EQ("Alex Guldemond", replace_all("Alex Lastname", "Lastname", "Guldemond"));
}

TEST(AddImplicitZeroes, Test) {
    EXPECT_EQ("0-a", add_implicit_zeroes("-a"));
    EXPECT_EQ("0+a+(0-a-b)", add_implicit_zeroes("+a+(-a-b)"));
}

TEST(RemoveZeroes, Test) {
    EXPECT_EQ("a+b+c+d", remove_spaces("a + b + c +    d"));
    EXPECT_EQ("a+b+c+d", remove_spaces("a+b+c+d"));
}

TEST(RemoveEnclosingParens, Test) {
    EXPECT_EQ("a+b+c", remove_enclosing_parens("(a+b+c)"));
    EXPECT_EQ("(a+b+c)*c", remove_enclosing_parens("((a+b+c)*c)"));
}

TEST(RemoveEnclosingParens, NoOp) {
    EXPECT_EQ("a+b+c", remove_enclosing_parens("a+b+c"));
    EXPECT_EQ("(a+b+c)*c", remove_enclosing_parens("(a+b+c)*c"));
}

TEST(JoinVector, Test) {
    EXPECT_EQ("a+b+c", join_vector("+",{"a", "b", "c"}));
    EXPECT_EQ("a/b/c", join_vector("/",{"a", "b", "c"}));
}

TEST(SplitString, Test) {
    std::vector<std::string> vec {"a","b","c"};
    EXPECT_EQ(vec, split_string("a+b+c", "+"));
}

TEST(BreakString, Addition) {
    std::vector<std::string> vec;
    std::string str ("a+b+(c+d*(e+f))");
    break_string(str, "+", vec, false);
    std::vector<std::string> test_vec {"a", "b", "c", "d*(e+f)"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Multiplication) {
    std::vector<std::string> vec;
    std::string str ("a*b*(c^(a*b))");
    break_string(str, "*", vec, false);
    std::vector<std::string> test_vec {"a", "b", "c^(a*b)"};
    EXPECT_EQ(test_vec, vec);
}

TEST(IsLeaf, Test) {
    node_ptr root;
    ASSERT_FALSE(is_leaf(root));
    root = std::make_shared<node>();
    root->token = "Hello";
    ASSERT_TRUE(is_leaf(root));
    root->children = { std::make_shared<node>(), std::make_shared<node>() };
    ASSERT_FALSE(is_leaf(root));
}

TEST(Kind, Undefined) {
    node_ptr root;
    EXPECT_EQ(UNDEFINED, kind(root));
}

TEST(Kind, Integer) {
    node_ptr root = std::make_shared<node>();
    root->token = "5";
    EXPECT_EQ(INTEGER, kind(root));
    root->token = "-24";
    EXPECT_EQ(INTEGER, kind(root));
}

TEST(Kind, Number) {
    node_ptr root = std::make_shared<node>();
    root->token = "1.5";
    EXPECT_EQ(NUMBER, kind(root));
}

TEST(Kind, Symbol) {
    node_ptr root = std::make_shared<node>();
    root->token = "a";
    EXPECT_EQ(SYMBOL, kind(root));
}

TEST(Kind, Fraction) {
    node_ptr root = std::make_shared<node>();
    root->token = "/";
    root->children = {std::make_shared<node>(), std::make_shared<node>()};
    root->children[0]->token = "5";
    root->children[1]->token = "20";
    EXPECT_EQ(FRACTION, kind(root));

    root = std::make_shared<node>();
    root->token = "*";
    root->children = {std::make_shared<node>(), std::make_shared<node>()};
    root->children[0]->token = "5";
    root->children[1]->token = "^";
    root->children[1]->children = {std::make_shared<node>(), std::make_shared<node>()};
    root->children[1]->children[0]->token = "20";
    root->children[1]->children[1]->token = "-1";
    EXPECT_EQ(FRACTION, kind(root));
}

TEST(Kind, Function) {
    node_ptr root = std::make_shared<node>();
    root->token = "Sin";
    root->children = { std::make_shared<node>() };
    root->children[0]->token = "0";
    EXPECT_EQ(FUNCTION, kind(root));
}

TEST(Kind, Add) {
    node_ptr root = std::make_shared<node>();
    root->token = "+";
    root->children = { std::make_shared<node>(),std::make_shared<node>(),std::make_shared<node>() };
    root->children[0]->token = "a";
    root->children[1]->token = "b";
    root->children[2]->token = "4";
    EXPECT_EQ(ADD, kind(root));
}

TEST(Kind, Subtract) {
    node_ptr root = std::make_shared<node>();
    root->token = "-";
    root->children = { std::make_shared<node>(),std::make_shared<node>()};
    root->children[0]->token = "a";
    root->children[1]->token = "b";
    EXPECT_EQ(SUBTRACT, kind(root));

    root->token = "+";
    root->children[1]->token = "*";
    root->children[1]->children = { std::make_shared<node>(),std::make_shared<node>()};
    root->children[1]->children[0]->token = "-1";
    root->children[1]->children[1]->token = "b";
    EXPECT_EQ(SUBTRACT, kind(root));
}

TEST(Kind, Multiply) {
    node_ptr root = std::make_shared<node>();
    root->token = "*";
    root->children = { std::make_shared<node>(),std::make_shared<node>()};
    root->children[0]->token = "a";
    root->children[1]->token = "b";
    EXPECT_EQ(MULTIPLY, kind(root));
}


TEST(Kind, Divide) {
    node_ptr root = std::make_shared<node>();
    root->token = "/";
    root->children = { std::make_shared<node>(),std::make_shared<node>()};
    root->children[0]->token = "a";
    root->children[1]->token = "b";
    EXPECT_EQ(DIVIDE, kind(root));

    root->token = "*";
    root->children[1]->token = "^";
    root->children[1]->children = { std::make_shared<node>(),std::make_shared<node>()};
    root->children[1]->children[0]->token = "b";
    root->children[1]->children[1]->token = "-1";
    EXPECT_EQ(DIVIDE, kind(root));
}

TEST(Kind, Power) {
    node_ptr root = std::make_shared<node>();
    root->token = "^";
    root->children = { std::make_shared<node>(),std::make_shared<node>()};
    root->children[0]->token = "a";
    root->children[1]->token = "b";
    EXPECT_EQ(POWER, kind(root));
}

TEST(NodeCompare, Test) {
    node_ptr tree1 = std::make_shared<node>();
    node_ptr tree2 = std::make_shared<node>();
    tree1->token = "a";
    tree2->token = "+";
    tree2->children = { std::make_shared<node>(),std::make_shared<node>()};
    tree2->children[0]->token = "a";
    tree2->children[1]->token = "b";
    EXPECT_TRUE(node_compare(tree1, tree2));
}

TEST(LoadTree, Test) {
    std::string str("b+a+c");
    node_ptr root = std::make_shared<node>();
    load_tree(str, root);
    ASSERT_EQ("+", root->token);
    ASSERT_EQ(3, root->children.size());
    ASSERT_EQ("a", root->children[0]->token);
    ASSERT_EQ("b", root->children[1]->token);
    ASSERT_EQ("c", root->children[2]->token);

    str = "a+b*(c+d*(e+f))";
    root = std::make_shared<node>();
    load_tree(str, root);
    ASSERT_EQ("+", root->token);
    ASSERT_EQ("a", root->children[0]->token);
    ASSERT_EQ("*", root->children[1]->token);
    ASSERT_EQ("b", root->children[1]->children[0]->token);
    ASSERT_EQ("+", root->children[1]->children[1]->token);
    ASSERT_EQ("c", root->children[1]->children[1]->children[0]->token);
    ASSERT_EQ("*", root->children[1]->children[1]->children[1]->token);
    ASSERT_EQ("d", root->children[1]->children[1]->children[1]->children[0]->token);
    ASSERT_EQ("+", root->children[1]->children[1]->children[1]->children[1]->token);
    ASSERT_EQ("e", root->children[1]->children[1]->children[1]->children[1]->children[0]->token);
    ASSERT_EQ("f", root->children[1]->children[1]->children[1]->children[1]->children[1]->token);
}

TEST(ChangeInverseMethod, Subtraction) {
    node_ptr root = std::make_shared<node>();
    load_tree("a-b", root);
    EXPECT_EQ("+", root->token);
    EXPECT_EQ("a", root->children[0]->token);
    EXPECT_EQ("-", root->children[1]->token);
    EXPECT_EQ("0", root->children[1]->children[0]->token);
    EXPECT_EQ("b", root->children[1]->children[1]->token);

    change_inverse_method(root);
    EXPECT_EQ("+", root->token);
    EXPECT_EQ("a", root->children[0]->token);
    EXPECT_EQ("*", root->children[1]->token);
    EXPECT_EQ("-1", root->children[1]->children[0]->token);
    EXPECT_EQ("b", root->children[1]->children[1]->token);
}

TEST(ChangeInverseMethod, Division) {
    node_ptr root = std::make_shared<node>();
    load_tree("a/b", root);
    EXPECT_EQ("*", root->token);
    EXPECT_EQ("a", root->children[0]->token);
    EXPECT_EQ("/", root->children[1]->token);
    EXPECT_EQ("1", root->children[1]->children[0]->token);
    EXPECT_EQ("b", root->children[1]->children[1]->token);

    change_inverse_method(root);
    EXPECT_EQ("*", root->token);
    EXPECT_EQ("a", root->children[0]->token);
    EXPECT_EQ("^", root->children[1]->token);
    EXPECT_EQ("b", root->children[1]->children[0]->token);
    EXPECT_EQ("-1", root->children[1]->children[1]->token);
}

TEST(Evaluate, Tests) {
    node_ptr root = std::make_shared<node>();
    load_tree("Sin[a-b]+c/d", root);
    EXPECT_NEAR(5, evaluate(root, {{ "a", 17.3}, {"b", 17.3}, {"c", 25}, {"d", 5}}), .001);
    
    root = std::make_shared<node>();
    load_tree("Cos[a*PI/2]", root);
    EXPECT_NEAR(1, evaluate(root, {{"a",0}}), .001);
    EXPECT_NEAR(0, evaluate(root, {{"a",1}}), .001);
    EXPECT_NEAR(-1, evaluate(root, {{"a",2}}), .001);
    EXPECT_NEAR(0, evaluate(root, {{"a",3}}), .001);
    EXPECT_NEAR(1, evaluate(root, {{"a",4}}), .001);
}

TEST(BuildStringFromTree, Tests) {
    node_ptr root = std::make_shared<node>();
    load_tree("Sin[a-b]+c/d", root);
    ASSERT_EQ("Sin[a+0-b]+c*1/d", build_string_from_tree(root));
    change_inverse_method(root);
    ASSERT_EQ("Sin[a+(-1)*b]+c*d^(-1)", build_string_from_tree(root));
}

TEST(CloneTree, Test) {
    node_ptr root = std::make_shared<node>();
    node_ptr clone = std::make_shared<node>();
    load_tree("a+b+c", root);
    clone_tree(root, clone);
    EXPECT_EQ(build_string_from_tree(root), build_string_from_tree(clone));

    root = std::make_shared<node>();
    root = std::make_shared<node>();
    load_tree("a+b-c", root);
    clone_tree(root, clone);
    EXPECT_EQ(build_string_from_tree(root), build_string_from_tree(clone));

    root = std::make_shared<node>();
    root = std::make_shared<node>();
    load_tree("a*b^c", root);
    clone_tree(root, clone);
    EXPECT_EQ(build_string_from_tree(root), build_string_from_tree(clone));

    root = std::make_shared<node>();
    root = std::make_shared<node>();
    load_tree("x^2+2*x+4", root);
    clone_tree(root, clone);
    EXPECT_EQ(build_string_from_tree(root), build_string_from_tree(clone));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

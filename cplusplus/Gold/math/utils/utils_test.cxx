#include "gtest/gtest.h"
#include "Gold/math/utils/utils.hpp"

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

TEST(IsStringVariable, True) {
    EXPECT_TRUE( is_string_var("Alex"));
    EXPECT_TRUE( is_string_var("guldemond"));
    EXPECT_TRUE( is_string_var("a1"));
    EXPECT_TRUE( is_string_var("a_1sdfg"));
}

TEST(IsStringVariable, False) {
    EXPECT_FALSE( is_string_var("Alex Guldemond"));
    EXPECT_FALSE( is_string_var("1abd"));
    EXPECT_FALSE( is_string_var("a+b"));
    EXPECT_FALSE( is_string_var("a.b,c"));
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

TEST(IsBinary, TrueCase) {
    EXPECT_TRUE(is_binary(1,"a+b"));
    EXPECT_TRUE(is_binary(1, "a-b"));
    EXPECT_TRUE(is_binary(1, "a*b"));
    EXPECT_TRUE(is_binary(1, "a/b"));
    EXPECT_TRUE(is_binary(1, "a^b"));
    EXPECT_TRUE(is_binary(5, "(a+b)-c"));
    EXPECT_TRUE(is_binary(2, "(a+b)-c"));
    EXPECT_TRUE(is_binary(10, "Func[a+b,a-c]"));
}

TEST(IsBinary, FalseCase) {
    EXPECT_FALSE(is_binary(0, "-a"));
    EXPECT_FALSE(is_binary(0, "-a"));
    EXPECT_FALSE(is_binary(2, "b*-a"));
    EXPECT_FALSE(is_binary(3, "b*(-a)"));
    EXPECT_FALSE(is_binary(7, "Func[a,-a]"));
    EXPECT_FALSE(is_binary(4 , "Sin[-a]"));
}


TEST(FindLowestPriority, Expressions) {
    try {
	ASSERT_EQ(1, find_lowest_priority("a+b"));
	ASSERT_EQ(5, find_lowest_priority("(a-b)*(d^c)"));
	ASSERT_EQ(1, find_lowest_priority("a^b^c"));
	ASSERT_EQ(9, find_lowest_priority("a*(x+y+x)+3"));
	ASSERT_EQ(-1, find_lowest_priority("a"));
	ASSERT_EQ(-2, find_lowest_priority("Sin[a*b/(c+d)]"));
	ASSERT_EQ(8, find_lowest_priority("Cos[x-y]+c"));
    }
    catch (const std::string& e) {
	EXPECT_EQ("", e); 
    }
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

TEST(RemoveEnclosingParens, Test) {
    EXPECT_EQ("a+b+c", remove_enclosing_parens("(a+b+c)"));
    EXPECT_EQ("(a+b+c)*c", remove_enclosing_parens("((a+b+c)*c)"));
}

TEST(RemoveEnclosingParens, NoOp) {
    EXPECT_EQ("a+b+c", remove_enclosing_parens("a+b+c"));
    EXPECT_EQ("(a+b+c)*c", remove_enclosing_parens("(a+b+c)*c"));
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

TEST(BreakString, Subtraction) {
    std::vector<std::string> vec;
    std::string str ("a-b-(c*(a-b))");
    break_string(str, "-", vec, false);
    std::vector<std::string> test_vec {"a", "(-1)*(b)", "(-1)*(c*(a-b))"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Subtraction2) {
    std::vector<std::string> vec;
    std::string str ("a-b-(c-d)");
    break_string(str, "-", vec, false);
    std::vector<std::string> test_vec {"a", "(-1)*(b)", "(-1)*(c)", "d"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Subtraction3) {
    std::vector<std::string> vec;
    std::string str ("a+(-b)");
    break_string(str, "-", vec, false);
    std::vector<std::string> test_vec {"a", "(-1)*(b)"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Subtraction4) {
    std::vector<std::string> vec;
    std::string str ("a+(-5)");
    break_string(str, "-", vec, false);
    std::vector<std::string> test_vec {"a", "-5"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Division) {
    std::vector<std::string> vec;
    std::string str ("a/b/(c+(a/b))");
    break_string(str, "/", vec, false);
    std::vector<std::string> test_vec {"a", "(b)^(-1)", "(c+(a/b))^(-1)"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Division2) {
    std::vector<std::string> vec;
    std::string str ("a/b/(c/d)");
    break_string(str, "/", vec, false);
    std::vector<std::string> test_vec {"a", "(b)^(-1)", "(c)^(-1)", "d"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Division3) {
    std::vector<std::string> vec;
    std::string str ("a*(1/b)");
    break_string(str, "/", vec, false);
    std::vector<std::string> test_vec {"a", "1", "(b)^(-1)"};
    EXPECT_EQ(test_vec, vec);
}

TEST(BreakString, Hybrid) {
    std::vector<std::string> vec;
    std::string str ( "a/(b*-c)");
    break_string(str, "/", vec, false);
    std::vector<std::string> test_vec {"a", "(b)^(-1)", "(-1)*(c)^(-1)" };
    EXPECT_EQ(test_vec, vec);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

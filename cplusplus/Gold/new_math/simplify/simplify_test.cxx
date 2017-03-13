#include "gtest/gtest.h"
#include "Gold/new_math/utils/utils.hpp"
#include "Gold/new_math/simplify/simplify.hpp"

#include <iostream>

using namespace Gold::math;

TEST(SimplifyRationalNumber, Test) {
    utils::node_ptr root = std::make_shared<utils::node>();
    utils::load_tree("2/4", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_number(root);
    EXPECT_EQ("1*2^(-1)", build_string_from_tree(root));

    root = std::make_shared<utils::node>();
    utils::load_tree("4/2", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_number(root);
    EXPECT_EQ("2", build_string_from_tree(root));
}

TEST(SimplifyRationalTree, Exponent) {
    utils::node_ptr root = std::make_shared<utils::node>();
    utils::load_tree("2^2", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("4", utils::build_string_from_tree(root));

    root = std::make_shared<utils::node>();
    utils::load_tree("2^0", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("1", utils::build_string_from_tree(root));
    
    root = std::make_shared<utils::node>();
    utils::load_tree("2^1", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("2", utils::build_string_from_tree(root));

    root = std::make_shared<utils::node>();
    utils::load_tree("2^1.1", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("2.143547", utils::build_string_from_tree(root));

    root = std::make_shared<utils::node>();
    utils::load_tree("1.1^2", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("1.210000", utils::build_string_from_tree(root));
    
    root = std::make_shared<utils::node>();
    utils::load_tree("1.1^2.2", root);
    utils::change_inverse_method(root);
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("1.233286", utils::build_string_from_tree(root));

    root = std::make_shared<utils::node>();
    utils::load_tree("((-1)*2)^1", root);
    utils::change_inverse_method(root); 
    simplify::simplify_rational_tree(root);
    EXPECT_EQ("(-1)*2", utils::build_string_from_tree(root));

    
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

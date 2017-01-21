#include "utils.hpp"
#include <iostream>


using namespace Gold::math::utils;

void print_tree_pre_order(node_ptr node) {
  if (node) {
    std::cout << node->token << "\t";
    if (!is_leaf(node)) {
      for (auto it = node->children.begin(); it != node->children.end(); it++) {
	print_tree_pre_order(*it);
      }
    }
  }
}

int main() {

  std::string good("-12345.7");
  std::string bad("dfgjdgh");

  if (is_string_num(good) && !is_string_num(bad)) {
    std::cout << "is_string_num success\n";
  } else {
    std::cerr << "is_string_num failure\n";
  }

  good = "(x+2)*(y/3)";
  bad = "((x+2)*(y/3))";

  if (has_uncontained_op(good, '*') && !has_uncontained_op(bad, '*')) {
    std::cout << "has_uncontained_op success\n";
  } else {
    std::cerr << "has_uncontained_op failure\n";
    std::cerr << good << " should be true: " << has_uncontained_op(good, '*') << "\n";
    std::cerr << bad << " should be false: " << has_uncontained_op(bad, '*') << "\n";
  }

  good = "((x+3)+(x^2)/(Sin[x+1]))";
  bad = "([x+3)+(x^2]/(Sin[x+1]))";

  if (are_parens_matched(good) && !are_parens_matched(bad)) {
    std::cout << "are_parens_matched success\n";
  } else {
    std::cerr << "are_parens_matched failure\n";
    std::cerr << good << " should be true: " << are_parens_matched(good) << "\n";
    std::cerr << bad << " should be false: " << are_parens_matched(bad) << "\n";
  }

  good = "Sin[x^2]";
  bad = "([x+3)+(x^2]/(Sin[x+1]))";

  if (is_function(good) && !is_function(bad)) {
    std::cout << "is_function success\n";
  } else {
    std::cerr << "is_function failure\n";
    std::cerr << good << " should be true: " << is_function(good) << "\n";
    std::cerr << bad << " should be false: " << is_function(bad) << "\n";
  }

  good = "x+1";
  bad = "x+1";

  if (needs_parens(good, "*", true ) && !needs_parens(bad, "+", false)) {
    std::cout << "needs_parens success\n";
  } else {
    std::cerr << "needs_parens failure\n";
    std::cerr << good << " should be true: " << needs_parens(good, "*", true) << "\n";
    std::cerr << bad << " should be false: " << needs_parens(bad, "+", false) << "\n";
  }

  good = "hello";
  bad = "goodbye";
  std::vector<std::string> vec {"hello", "my", "friend"};

  if (contains(good,vec) == 0 && contains(bad, vec) == -1) {
    std::cout << "contains success\n";
  } else {
    std::cerr << "contains failure\n";
    std::cerr << good << " should be 0: " << contains(good,vec) << "\n";
    std::cerr << bad << " should be -1: " << contains(bad,vec) << "\n";
  }

  good = "(x+3)*(x-2)";
  bad = "(x+3)*(x-3)";

  if (search(good,1, '+', '-') == 8 && search(bad, 0 , '*', '/') == 5) {
    std::cout << "search success\n";
  } else {
    std::cerr << "search failure\n";
    std::cerr << good << " should be 8: " << search(good,1,'+','-') << "\n";
    std::cerr << bad << " should be 5: " << search(bad,0,'*', '*') << "\n";
  }

  if (find_lowest_priority(good) == 5) {
    std::cout << "find_lowest_priority success\n";
  } else {
    std::cerr << "find_lowest_priority failure\n";
    std::cerr << good << " should be 5: " << find_lowest_priority(good) << "\n";
  }

  good = "x+3*x/(12^x)";
  bad = "x-1";
  append_with_parens(good, bad, "*", true);
  if (good == "x+3*x/(12^x)*(x-1)") {
    std::cout << "append_with_parens success\n";
  } else {
    std::cerr << "append_with_parens failure\n";
    std::cerr << good << " should be x+3*x/(12^x)*(x-1): " << good << "\n";
  }

  good = "Hello todd my name is todd, todd todd";

  if (replace_all(good, "todd", "tedly") == "Hello tedly my name is tedly, tedly tedly") {
    std::cout << "replace_all success\n";
  } else {
    std::cerr << "replace_all failure\n";
    std::cerr << good << " should be Hello tedly my name is tedly, tedly tedly: " << replace_all(good, "todd", "tedly") << "\n";
  }
  
  good = "+2(1+(-2))";
  
  if (add_implicit_zeroes(good) == "0+2(1+(0-2))") {
    std::cout << "add_implicit_zeroes success\n";
  } else {
    std::cerr << "add_implicit_zeroes failed\n";
    std::cerr << good << " should be 0+2(1+(0-2)): " << add_implicit_zeroes(good) << "\n";
  }

  good = " Hello Chum ";
  
  if (remove_spaces(good) == "HelloChum") {
    std::cout << "remove_spaces success\n";
  } else {
    std::cerr << "remove_spaces failed\n";
    std::cerr << good << " should be HelloChum: " << remove_spaces(good) << "\n";
  }

  good = "(hello chum)";
  bad = "hello chum";
  if (remove_enclosing_parens(good) == "hello chum" || 
      remove_enclosing_parens(bad) == "hello chum") {
    std::cout << "remove_enclosing_parens success\n";
  } else {
    std::cerr << "remove_enclosing_parens failed\n";
    std::cerr << good << " should be hello chum: " << remove_enclosing_parens(good) << "\n";
    std::cerr << bad  << " should be hello chum: " << remove_enclosing_parens(bad) << "\n";
  }

  vec = {"hello", "my", "name", "is", "alex"};
  if ( join_vector("|", vec) == "hello|my|name|is|alex" ) {
      std::cout << "join_vector success\n";
  } 
  else {
      std::cerr << "join_vector failed\n";
      std::cerr << " should be hello|my|name|is|alex: " << join_vector("|", vec);
  }

  std::cerr << "\n\n===========Node Tests==============\n\n";

  node_ptr good_node = std::make_shared<node>();
  node_ptr bad_node = std::make_shared<node>();
  bad_node->children.push_back(good_node);

  if (is_leaf(good_node) && ! is_leaf(bad_node)) {
    std::cout << "is_leaf success\n";
  } else {
    std::cerr << "is_leaf failed\n";
    std::cerr << good_node <<  " Should be 1: " << is_leaf(good_node) << "\n";
  }

  node_ptr tree = std::make_shared<node>();
  load_tree("x^2+3*x+4", tree);
  std::cout << "x^2+3*x+4" << "\n";
  print_tree_pre_order(tree);
  std::cout << "\n";

  std::cout << "Testing 0, should be 4: " << evaluate(tree, {{"x",0}}) << "\n";
  std::cout << "Testing 1, should be 8: " << evaluate(tree, {{"x",1}}) << "\n";
  std::cout << "Testing -1, should be 2: " << evaluate(tree, {{"x",-1}}) << "\n";
  std::cout << "Testing -2, should be 2: " << evaluate(tree, {{"x",-2}}) << "\n";
  std::cout << "Testing 2, should be 14: " << evaluate(tree, {{"x", 2}}) << "\n";

  std::cout << "\n";
  tree.reset(new node);
  load_tree("Sin[x+PI/2]",tree);
  print_tree_pre_order(tree);
  std::cout << "\n";

  double pi = symbols_table["PI"];
  
  std::cout << "Testing 0, should be 1: " << evaluate(tree, {{"x",0}}) << "\n";
  std::cout << "Testing PI/2, should be 0: " << evaluate(tree, {{"x",pi/2.0}}) << "\n";
  std::cout << "Testing PI, should be -1: " << evaluate(tree, {{"x",pi}}) << "\n";
  std::cout << "Testing 3*PI/2, should be 0: " << evaluate(tree, {{"x",3*pi/2}}) << "\n";

  std::cout << "\n\n";

  tree.reset(new node);
  load_tree("x+3",tree);
  std::cout << "Testing buils_string_from_tree, should be x+3: " << build_string_from_tree(tree) << "\n";
  
  tree.reset(new node);
  load_tree("x+3+(3*6)",tree);
  std::cout << "Testing buils_string_from_tree, should be x+3+3*6: " << build_string_from_tree(tree) << "\n";

  tree.reset(new node);
  load_tree("Func[a,b, alex]+(x+3)*(x+4)*(x+5)/(x^2-3)",tree);
  std::cout << "Testing buils_string_from_tree, should be Func[a,b,alex]+(x+3)*(x+4)*(x+5)/(x^2-3): " << build_string_from_tree(tree) << "\n";
  

  tree.reset(new node);
  load_tree("3^2^3",tree);
  try {
      std::cout << "Testing 3^2^3: " << build_string_from_tree(tree) << " = " << evaluate(tree, {}) << "\n";
  } catch (const std::string& e) {
      std::cerr << "Exception thrown: " <<e << "\n";
  }

  
  tree.reset(new node);
  load_tree("(3^2)^3",tree);
  print_tree_pre_order(tree);
  std::cout << "\n";
  try {
      std::cout << "Testing (3^2)^3: " << build_string_from_tree(tree) << " = " << evaluate(tree, {}) << "\n";
  } catch (const std::string& e) {
      std::cerr << "Exception thrown: " <<e << "\n";
  }
  

  tree.reset(new node);
  load_tree("1/3^2",tree);
  try {
      std::cout << "Testing 1/3^2: " << build_string_from_tree(tree) << " = " << evaluate(tree, {}) << "\n";
  } catch (const std::string& e) {
      std::cerr << "Exception thrown: " <<e << "\n";
  }
}
  

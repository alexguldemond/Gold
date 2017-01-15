#include "utils.hpp"
#include <iostream>

int main() {

  std::string good("-12345.7");
  std::string bad("dfgjdgh");

  if (Gold::math::utils::is_string_num(good) && !Gold::math::utils::is_string_num(bad)) {
    std::cout << "is_string_num success\n";
  } else {
    std::cerr << "is_string_num failure\n";
  }

  good = "(x+2)*(y/3)";
  bad = "((x+2)*(y/3))";

  if (Gold::math::utils::has_uncontained_op(good, '*') && !Gold::math::utils::has_uncontained_op(bad, '*')) {
    std::cout << "has_uncontained_op success\n";
  } else {
    std::cerr << "has_uncontained_op failure\n";
    std::cerr << good << " should be true: " << Gold::math::utils::has_uncontained_op(good, '*') << "\n";
    std::cerr << bad << " should be false: " << Gold::math::utils::has_uncontained_op(bad, '*') << "\n";
  }

  good = "((x+3)+(x^2)/(Sin[x+1]))";
  bad = "([x+3)+(x^2]/(Sin[x+1]))";

  if (Gold::math::utils::are_parens_matched(good) && !Gold::math::utils::are_parens_matched(bad)) {
    std::cout << "are_parens_matched success\n";
  } else {
    std::cerr << "are_parens_matched failure\n";
    std::cerr << good << " should be true: " << Gold::math::utils::are_parens_matched(good) << "\n";
    std::cerr << bad << " should be false: " << Gold::math::utils::are_parens_matched(bad) << "\n";
  }

  good = "Sin[x^2]";
  bad = "([x+3)+(x^2]/(Sin[x+1]))";

  if (Gold::math::utils::is_function(good) && !Gold::math::utils::is_function(bad)) {
    std::cout << "is_function success\n";
  } else {
    std::cerr << "is_function failure\n";
    std::cerr << good << " should be true: " << Gold::math::utils::is_function(good) << "\n";
    std::cerr << bad << " should be false: " << Gold::math::utils::is_function(bad) << "\n";
  }

  good = "x+1";
  bad = "x+1";

  if (Gold::math::utils::needs_parens(good, "*", true ) && !Gold::math::utils::needs_parens(bad, "+", false)) {
    std::cout << "needs_parens success\n";
  } else {
    std::cerr << "needs_parens failure\n";
    std::cerr << good << " should be true: " << Gold::math::utils::needs_parens(good, "*", true) << "\n";
    std::cerr << bad << " should be false: " << Gold::math::utils::needs_parens(bad, "+", false) << "\n";
  }

  good = "hello";
  bad = "goodbye";
  std::vector<std::string> vec {"hello", "my", "friend"};

  if (Gold::math::utils::contains(good,vec) == 0 && Gold::math::utils::contains(bad, vec) == -1) {
    std::cout << "contains success\n";
  } else {
    std::cerr << "contains failure\n";
    std::cerr << good << " should be 0: " << Gold::math::utils::contains(good,vec) << "\n";
    std::cerr << bad << " should be -1: " << Gold::math::utils::contains(bad,vec) << "\n";
  }

  good = "(x+3)*(x-2)";
  bad = "(x+3)*(x-3)";

  if (Gold::math::utils::search(good,1, '+', '-') == 8 && Gold::math::utils::search(bad, 0 , '*', '/') == 5) {
    std::cout << "search success\n";
  } else {
    std::cerr << "search failure\n";
    std::cerr << good << " should be 8: " << Gold::math::utils::search(good,1,'+','-') << "\n";
    std::cerr << bad << " should be 5: " << Gold::math::utils::search(bad,0,'*', '*') << "\n";
  }

  if (Gold::math::utils::find_lowest_priority(good) == 5) {
    std::cout << "find_lowest_priority success\n";
  } else {
    std::cerr << "find_lowest_priority failure\n";
    std::cerr << good << " should be 5: " << Gold::math::utils::find_lowest_priority(good) << "\n";
  }

 
}
  

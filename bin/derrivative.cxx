#include "Gold/math/calculus/calculus.hpp"
#include "Gold/argparser/argparser.hpp"
#include <iostream>

int main(int argc, char** argv) {
  Gold::argparser parser(2);
  try {
    parser.parse_args(argc, argv);
  } catch (std::string& e) {
    std::cerr << e << "\n";
    return 1;
  } 
  
  Gold::math::expression expr;
  try {
    expr = Gold::math::expression(parser.get_pos_arg<std::string>(0), {parser.get_pos_arg<std::string>(1) } );
    std::cout << Gold::math::calculus::derrivative(expr, parser.get_pos_arg<std::string>(1)).build_expression() << "\n";
  } catch (Gold::math::expression_exception& e) {
    std::cerr << e.error() << "\n";
    return 1;
  }
  return 0;
}

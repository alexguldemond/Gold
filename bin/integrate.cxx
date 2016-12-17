#include "Gold/math/calculus/calculus.hpp"
#include "Gold/argparser/argparser.hpp"
#include <iostream>

int main(int argc, char** argv) {
  Gold::argparser parser(4);
  try {
    parser.parse_args(argc, argv);
  } catch (std::string& e) {
    std::cerr << e << "\n";
    return 1;
  } 
  
  Gold::math::expression expr;
  try {
    std::string var = parser.get_pos_arg<std::string>(1);
    double x1 = parser.get_pos_arg<double>(2);
    double x2 = parser.get_pos_arg<double>(3);
    expr = Gold::math::expression(parser.get_pos_arg<std::string>(0), { var } );
    std::cout << Gold::math::calculus::numerically_integrate(expr, x1, x2) << "\n";
  } catch (Gold::math::expression_exception& e) {
    std::cerr << e.error() << "\n";
    return 1;
  }
  return 0;
}

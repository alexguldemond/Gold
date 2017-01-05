#ifndef CALCULUS_HPP
#define CALCULUS_HPP

#if defined MATH
#define MATH_COMMON_DLLSPEC Q_DECL_EXPORT
#else
#define MATH_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

#include "Gold/math/expression/expression.hpp"
#include "Gold/math/utils/utils.hpp"

namespace Gold {
  namespace math {
    
    using Gold::math::utils::KeyString;
    using Gold::math::utils::Node;
    class calculus {
    private:
    public:
      calculus()=delete;
      calculus(const calculus& other)=delete;
      calculus* operator=(const calculus& other)=delete;                         //Never instantiate the class
      
      static expression derrivative(const expression& expr, const std::string& var);
      static double numerically_integrate(const expression& expr, double x1, double x2, double dx = 0.00001);
      
    private:
      static std::string _derrivative(Node* node, const std::string& var);
      static std::string d_string(const std::string& str, const std::string& var);
      static std::map<std::string,std::string> d_table;
    };
  }
}
#endif

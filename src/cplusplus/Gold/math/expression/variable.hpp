#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "Gold/math/expression/expression.hpp"

namespace Gold {
    namespace math {
	
	class variable: public expression {
	public:
	    variable();
	    explicit variable(const std::string& expr);
	    variable(const variable& other);
	    variable(variable&& other);
	    virtual ~variable() { }
	    variable& operator=(const variable& other);
	    variable& operator=(variable&& other);

	private:
	    std::string _check_variable(const std::string& str);
	};
	
    }
}

#endif

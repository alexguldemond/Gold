#include "Gold/math/variable.hpp"

namespace Gold {
    namespace math {

	variable::variable() { }

	variable::variable(const std::string& str) : expression(_check_variable(str)) { }

	variable::variable(const variable& other) : expression(other) { }

	variable::variable(variable&& other) : expression(other) { }

	variable& variable::operator=(const variable& other) {
	    expression::operator=(other);
	    return *this;
	}

	variable& variable::operator=(variable&& other) {
	    expression::operator=(other);
	    return *this;
	}

	std::string variable::_check_variable(const std::string& str) {
	    if (!utils::is_string_var(str)) {
		throw std::string(str).append(" can't be used as a variable");
	    }
	    return str;
	}

    }
}

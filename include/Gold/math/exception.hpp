#ifndef GOLD_MATH_EXCEPTION_HPP
#define GOLD_MATH_EXCEPTION_HPP

#include <string>
#include <exception>

#define EXTEND_EXCEPTION(derrived, base) class derrived : public base { \
public: \
 explicit derrived(const std::string& what_arg) : base(what_arg) { } \
    };

namespace Gold {
    namespace math {
	
	class exception : public std::exception {
	public:
	    explicit exception(const std::string& what_arg) : str(what_arg) { }
	    virtual const char* what() const throw() { return str.c_str(); }
	private:
	    std::string str;
	};

	EXTEND_EXCEPTION(invalid_string, exception)

	EXTEND_EXCEPTION(invalid_expression, invalid_string);
	
	EXTEND_EXCEPTION(invalid_variable, invalid_string);

	EXTEND_EXCEPTION(invalid_operation, invalid_string);

	EXTEND_EXCEPTION(undefined_expression, exception);

	EXTEND_EXCEPTION(variable_not_found, exception);

	EXTEND_EXCEPTION(invalid_node, exception);
   }
}

#endif

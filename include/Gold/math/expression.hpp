#ifndef EXPRESSION_HP
#define EXPRESSION_HP

#include <string>
#include <sstream>
#include <map>
#include <functional>
#include <vector>
#include <exception>
#include "Gold/math/node.hpp"
#include "Gold/math/utils.hpp"

namespace Gold {
    namespace math {

	class expression {
	public:
	    expression();
	    explicit expression(const std::string& expr);
	    explicit expression(int value) : expression(std::to_string(value)) { }
	    explicit expression(double value) : expression(std::to_string(value)) { }
	    expression(const expression& other);
	    expression(expression&& other);
	    virtual ~expression() { }
	    expression& operator=(const expression& other);
	    expression& operator=(expression&& other);

	    virtual double evaluate(const std::map<std::string, double>& args = {}) const;
	    virtual expression operator()(const std::map<std::string, expression>& args = {}) const; 
	    virtual bool defined() const;
	    virtual std::string string() const { 
		if (string_value == "") {
		    string_value = root->string();
		}
		return string_value; 
	    }

	    friend expression operator+(const expression& lhs, const expression& rhs);
	    friend expression operator-(const expression& expr);
	    friend expression operator-(const expression& lfs, const expression& rhs);
	    friend expression operator*(const expression& lhs, const expression& rhs);
	    friend expression operator/(const expression& lhs, const expression& rhs);
	    friend expression pow(const expression& lhs, const expression& rhs);
	    friend expression derivative(const expression& expr,const std::string& var);
	private:
	    node::base_node::ptr root;
	    mutable std::string string_value;
	};
	
    }
}

	   
#endif

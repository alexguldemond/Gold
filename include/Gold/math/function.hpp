#include "Gold/math/expression.hpp"
#include "Gold/math/variable.hpp"

namespace Gold {
    namespace math {
	
	class function {
	private:
	    std::vector<variable> variables;
	    expression rule;
	public:
	    function(const variable& var, const expression& expr) : variables(1,var), rule(expr) { } 
	    function(const variable& var, expression&& expr) : variables(1,var), rule(expr) { }
	    function(variable&& var, const expression&& expr) : variables(1,var), rule(expr) { }
	    function(variable&& var, expression&& expr) : variables(1,var), rule(expr) { }

	    function(const std::vector<variable>& vars, const expression& expr) : variables(vars), rule(expr) { }
	    function(const std::vector<variable>& vars, expression&& expr) : variables(vars), rule(expr) { }

	    virtual ~function() { }

	    expression operator()(const std::vector<expression>& replacements);
	    double evaluate(const std::vector<double>& values);
	};
    }
}

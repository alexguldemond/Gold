#include <stdexcept>
#include "Gold/math/function.hpp"

namespace Gold {
    namespace math {
	
	expression function::operator()(const std::vector<expression>& replacements) {
	    if (replacements.size() != this->variables.size() ) {
		throw std::length_error("Incorrect number of variables");
	    }
	    std::map<std::string, expression> changes;
	    for (std::size_t i = 0; i != replacements.size(); i++) {
		changes.insert( std::pair<std::string, expression>(this->variables[i].string(), replacements[i]) );
	    }
	    return rule(changes);	    
	}

	double function::evaluate(const std::vector<double>& values) {
	    if (values.size() != this->variables.size() ) {
		throw std::length_error("Incorrect number of variables");
	    }
	    std::map<std::string, double> changes;
	    for (std::size_t i = 0; i != values.size(); i++) {
		changes.insert( std::pair<std::string, double>(this->variables[i].string(), values[i]) );
	    }
	    return rule.evaluate(changes);	    
	}   
    }
}

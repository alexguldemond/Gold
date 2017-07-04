#include "Gold/math/expression.hpp"
#include <iostream>

namespace Gold {
    namespace math {
	
	expression::expression() {
	    //Intentionally empty
	}
	
	expression::expression(const std::string& expr) {
	    std::string str = expr;
	    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
	    str = utils::remove_enclosing_parens(str);
	    if ( !utils::are_parens_matched(str) ) {
		throw std::string("Mismatched parentheses");
	    }
	    root = node::make_tree(str);
	}
	
	expression::expression(const expression& other) {
	    if (&other != this) {
		root = node::base_node::ptr(other.root->clone());
		string_value = "";
	    }
	}
	
	expression::expression(expression&& other) {
	    if (&other != this) {
		root = node::base_node::ptr(std::move(other.root));
		string_value = std::move(other.string_value);
	    }
	}
	
	expression& expression::operator=(const expression& other) {
	    if (&other != this) {
		root = node::base_node::ptr(other.root->clone());
		string_value = "";
	    }
	    return *this;
	}
	
	expression& expression::operator=(expression&& other) {
	    if (&other != this) {
		root = node::base_node::ptr(std::move(other.root));
		string_value = std::move(other.string_value);
	    }
	    return *this;
	}
	
	double expression::evaluate(const std::map<std::string, double>& args) const {
	    if (!defined()) {
		throw std::string("Undefined evaluation");
	    }
	    return root->evaluate(args);
	}
	
	bool expression::defined() const {
	    return bool(root && !root->is_undefined());
	}
	
        expression expression::operator()(const std::map<std::string, expression>& args) const {
	    if (!defined()) {
		throw std::string("Undefined evaulation");
	    }
	    
	    if (!std::all_of(args.begin(), args.end(),
			     [](const auto& pair) {
				 return Gold::math::utils::is_string_var(pair.first);
			     })
		) {
		throw std::string("Passed in change is not a variable");
	    }

	    if (std::any_of(args.begin(), args.end(), 
			    [](const auto& pair) {
				return !pair.second.defined();
			    })
		) {
		throw std::string("Cannot use an undefined expression to change variables");
	    }
	    
	    std::map<std::string, node::base_node::ptr> replacements;
	    for (auto iter = args.begin(); iter != args.end(); iter++) {
		replacements.insert( std::pair<std::string, node::base_node::ptr>(iter->first, node::base_node::ptr(iter->second.root->clone())));
	    }
	    expression result;
	    result.root = root->change_variables(replacements);
	    return result;
	}

	expression expression::commutative_operator(const expression& lhs, const expression& rhs, const std::string& operation) {
	    node::base_node::ptr left = node::base_node::ptr(lhs.root->clone());
	    node::base_node::ptr right = node::base_node::ptr(rhs.root->clone());
	    expression result;
	    if (left->get_token() == "+" && right->get_token() == "+") {
		for (auto iter = right->children.begin(); iter != right->children.end(); iter++) {
		    left->children.push_back(std::move(*iter));
		}
		result.root = std::move(left);
	    }
	    else if (left->get_token() == "+") {
		left->children.push_back(std::move(right));
		result.root = std::move(left);
	    }
	    else if (right->get_token() == "+") {
		auto iter = right->children.begin();
		right->children.insert(iter, std::move(left));
		result.root = std::move(right);
	    }
	    else {
		node::base_node::vec vector;
		vector.reserve(2);
		vector.push_back(std::move(left));
		vector.push_back(std::move(right));
		node::add::ptr newRoot = std::make_unique<node::add>(vector);
		result.root = std::move(newRoot);
	    }
	    return result;
	}

	expression operator+(const expression& lhs, const expression& rhs) {
	    return expression::commutative_operator(lhs, rhs, "+");
	}

	expression operator-(const expression& expr) {
	    node::base_node::ptr right = node::base_node::ptr(expr.root->clone());
	    if (right->get_token() == "*") {
		right->children.insert(right->children.begin(), std::make_unique<node::integer>(-1));
	    }
	    else {
		node::base_node::vec vector;
		vector.reserve(2);
		vector.push_back(std::make_unique<node::integer>(-1));
		vector.push_back(std::move(right));
		right = std::make_unique<node::multiply>(vector);
	    }
	    
	    expression result;
	    result.root = std::move(right);
	    return result;
	}

	
	expression operator-(const expression& lhs, const expression& rhs) {
	    return lhs + (-rhs);
	}

	expression operator*(const expression& lhs, const expression& rhs) {
	    return expression::commutative_operator(lhs, rhs, "*");
	}
	
	expression operator/(const expression& lhs, const expression& rhs) {
	    node::base_node::ptr left = node::base_node::ptr(lhs.root->clone());
	    node::base_node::ptr right = node::base_node::ptr(rhs.root->clone());
	    node::quotient::ptr result_node = std::make_unique<node::quotient>(*left, *right);
	    expression result;
	    result.root = std::move(result_node);
	    return result;
	}

	expression pow(const expression& lhs, const expression& rhs) {
	    node::base_node::ptr left = node::base_node::ptr(lhs.root->clone());
	    node::base_node::ptr right = node::base_node::ptr(rhs.root->clone());
	    node::power::ptr result_node = std::make_unique<node::power>(*left, *right);
	    expression result;
	    result.root = std::move(result_node);
	    return result;
	}

	expression derivative(const expression& expr, const std::string& var) {
	    node::base_node::ptr derivative = expr.root->derivative(var);
	    expression result;
	    result.root = std::move(derivative);
	    return result;
	}
	
    }
}

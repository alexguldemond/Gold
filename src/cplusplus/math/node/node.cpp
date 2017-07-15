#include "Gold/math/node.hpp"
#include "Gold/math/utils.hpp"
#include "Gold/math/exception.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cmath>
#include <iostream>
#include <iterator>

namespace Gold {
    namespace math {
	namespace node {

	    operation::operation(const base_node::vec& _children) {
		this->children.reserve(_children.size());
		for (auto iter = _children.begin(); iter != _children.end(); iter++) {
		    append_clone(**iter);
		}
	    }

	    operation::operation(base_node::vec&& _children) {
		this->children = std::move(_children);
	    }

	    operation::operation(const operation& other) {
		this->children.reserve(other.size());
		for (auto iter = other.begin(); iter != other.end(); iter++) {
		    append_clone(**iter);
		}
	    }

	    operation::operation(operation&& other) {
		this->children = std::move(other.children);
	    }

	    operation& operation::operator=(const operation& other) {
		if (&other != this) {
		    this->children.clear();
		    for (auto iter = other.begin(); iter != other.end(); iter++) {
			append_clone(**iter);
		    }
		}
		return *this;
	    }
	    
	    operation& operation::operator=(operation&& other) {
		if (&other != this) {
		    this->children = std::move(other.children);
		}
		return *this;
	    }
			
	    base_node::ptr base_node::numerator() const {
		return base_node::ptr(this->clone());
	    }

	    base_node::ptr base_node::denominator() const {
		return std::make_unique<integer>(1);
	    }

	    base_node::ptr base_node::base() const {
		return base_node::ptr(this->clone());
	    }

	    base_node::ptr base_node::exponent() const {
		return std::make_unique<integer>(1);
	    }

	    inverse::ptr inverse::make_inverse(const base_node& denominator) {
		inverse::ptr inverse_node = std::make_unique<inverse>();
		inverse_node->children.reserve(2);
		inverse_node->append( base_node::ptr(denominator.clone()) );
		inverse_node->append( std::make_unique<integer>(-1) );
		return inverse_node;
	    }

	    base_node::ptr inverse::denominator() const {
		if (size() != 2) {
		    throw invalid_node("inverse node not initialized correctly");
		}
		return base_node::ptr(children[0]->clone());
	    }
	    
	    quotient::quotient(const base_node& numerator, const base_node& denominator) {
		children.clear();
		children.reserve(2);
		append( base_node::ptr(numerator.clone()) );
		append( inverse::make_inverse(denominator) );
	    }

	    base_node::ptr quotient::numerator() const { 
		if (size() != 2) {
		    throw invalid_node("Quotient node not initialized correctly");
		}
		return base_node::ptr( child(0).clone() ); 
	    }
	    
	    base_node::ptr quotient::denominator() const { 
		if ( size() != 2 || child(1).size() != 2) {
		    throw invalid_node("Quotient node not initialized correctly");
		}
		operation& power_node = static_cast<operation&>(child(1)); 
		return base_node::ptr( power_node.child(0).clone() );
	    }
	    
	    rational::rational(int numerator, int denominator) {
		    children.clear();
		    children.reserve(2);
		    append(std::make_unique<integer>(numerator));
		    append(inverse::make_inverse( integer(denominator) ) );
	    }

	    /*************************************************
	     *
	     * Definitions of evaluate
	     *
	     ************************************************/
	    double add::evaluate(const std::map<std::string, double>& args) const {
		if (is_leaf()) {
			throw invalid_node("Add node not initialized with children");
		}
		double sum = 0;
		for (auto iter = begin(); iter != end(); iter++) {
		    sum += (*iter)->evaluate(args);
		}
		return sum;
	    }

	    double multiply::evaluate(const std::map<std::string, double>& args) const {
		if (is_leaf()) {
			throw invalid_node("multiply node not initialized with children");
		}
		double product = 1;
		for (auto iter = begin(); iter != end(); iter++) {
		    product *= (*iter)->evaluate(args);
		}
		return product;
	    }	
	    
	    power::power(const base_node& base, const base_node& exponent) {
		children.reserve(2);
		append( std::unique_ptr<base_node>(base.clone()));
		append( std::unique_ptr<base_node>(exponent.clone()));
	    }

	    base_node::ptr power::base() const { 
		if (is_leaf()) {
		    throw invalid_node("Power node not initialized with children");
		}
		return base_node::ptr( child(0).clone() ); 
	    }

	    base_node::ptr power::exponent() const {
		if (is_leaf()) {
		    throw invalid_node("Power node not initialized with children");
		}
		return base_node::ptr( child(1).clone() ); 
	    }

	    double power::evaluate(const std::map<std::string, double>& args) const {
		if (size() != 2) {
			throw invalid_node("Power node initialized with more or less than two children");
		}
		double base = this->base()->evaluate(args);
		double expo = this->exponent()->evaluate(args);	
		return std::pow(base, expo);
	    }

	    double function::evaluate(const std::map<std::string, double>& args) const {
		if (is_leaf()) {
			throw invalid_node("Function node not initialized with children");
		}
		if ( built_in_functions.find(get_token()) != built_in_functions.end()) {
		    if (size() != 1) {
			throw invalid_node("Built in functions take only one argument");
		    }
		    else {
			double value = child(0).evaluate(args);
			return built_in_functions.at(get_token())(value);
		    }
		}
		else {
		    throw std::invalid_argument(get_token().append(" not found in functions"));
		}
		return 0;
	    }

	    double integer::evaluate(const std::map<std::string, double>& args) const {
		return token;
	    }
	    
	    double number::evaluate(const std::map<std::string, double>& args) const {
		return token;
	    }
	    
	    double variable::evaluate(const std::map<std::string, double>& args) const {
		auto iter = args.find(token);
		if (args.find(token) == args.end() ) {
		    throw std::invalid_argument("Variable not found\n");
		}
		return iter->second;
	    }

	    /******************************************************
	     * Build string operations
	     *****************************************************/
	    std::string add::string() const {
		if (is_leaf()) {
		    return "";
		}
		std::string token = get_token();
		std::string first = child(0).string();
		for (auto iter = begin()+1; iter != end(); iter++) {
		    Gold::math::utils::append_with_parens(first, (*iter)->string(), token, true);
		}
		return first;
	    }
	    
	    std::string multiply::string() const {
		if (is_leaf()) {
		    return "";
		}
		std::string token = get_token();
		std::string first = child(0).string();
		if (child(0).get_token() == "+") {
		    first = std::string("(").append(first).append(")");
		}
		for (auto iter = begin()+1; iter != end(); iter++) {
		    Gold::math::utils::append_with_parens(first, (*iter)->string(), token, true);
		}
		return first;
	    }

	    std::string power::string() const {
		if (is_leaf()) {
		    return "";
		}
		std::string token = get_token();
		std::string first = child(0).string();
		if (child(0).get_token() == "+" || child(0).get_token() == "*" || child(0).get_token() == "^") {
		    first = std::string("(").append(first).append(")");
		}
		for (auto iter = begin()+1; iter != end(); iter++) {
		    Gold::math::utils::append_with_parens(first, (*iter)->string(), token, true);
		}
		return first;
	    }

	    std::string function::string() const {
		if (is_leaf()) {
		    return "";
		}
		
		std::string str = get_token().append("[");
		std::vector<std::string> subStrings;
		subStrings.reserve(size());
		for (auto iter = begin(); iter != end(); iter++) {
		    subStrings.push_back( (*iter)->string() );
		}
		str.append( boost::algorithm::join(subStrings, ", ") ).append("]");
		return str;
	    }

	    base_node::ptr add::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw invalid_node("Node is undefined, cannot take derivative");
		}
		base_node::vec derivative_vector;
		derivative_vector.reserve(size());
		for (auto iter = begin(); iter != end(); iter++) {
		    auto derivative_node = (*iter)->derivative(var);
		    if (!derivative_node->is_zero()) {
			derivative_vector.push_back(std::move(derivative_node) );
		    }
		}
		return std::make_unique<add>(derivative_vector);
	    }

	    base_node::ptr multiply::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw invalid_node("Node is undefined, cannot take derivative");
		}
		if (is_zero() || is_one() || is_minus_one()) {
		    return std::make_unique<integer>(0);
		}
		base_node::vec derivative_vector;
		for (auto iter = begin(); iter != end(); iter++) {
		    base_node::vec sub_vector;
		    bool is_derivative_zero = false;
		    for (auto jter = begin(); jter != end(); jter++) {
			if (iter == jter) {
			    base_node::ptr derivative_node = (*jter)->derivative(var);
			    if ( (is_derivative_zero = derivative_node->is_zero()) ) break;
			    if (!derivative_node->is_one()) sub_vector.push_back(std::move( derivative_node ) );
			}
			else {
			    auto clone = (*jter)->clone();
			    if (!clone->is_one()) sub_vector.push_back(base_node::ptr(clone) );
			}
		    }
		    if (!is_derivative_zero) {
			multiply::ptr sub_node = std::make_unique<multiply>(sub_vector);
			derivative_vector.push_back(std::move(sub_node) );
		    }
		}
		return std::make_unique<add>(derivative_vector);
	    }

	    base_node::ptr power::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw invalid_node("Power node is undefined");
		}
		if (is_zero() || is_one() || is_minus_one()) {
		    return std::make_unique<integer>(0);
		}
		
		base_node::ptr base_prime = this->base()->derivative(var);
		base_node::ptr expo_prime = this->exponent()->derivative(var);
		multiply::ptr derivative = std::make_unique<multiply>();
		if (base_prime->is_zero() && expo_prime->is_zero()) {
		    return std::make_unique<integer>(0);
		}
		else if (expo_prime->is_zero()) {
		    power::ptr power_node = std::make_unique<power>();
		    power_node->append(this->base());
		    add::ptr exponent_node = std::make_unique<add>();
		    exponent_node->append(this->exponent());
		    exponent_node->append(std::make_unique<integer>(-1));
		    power_node->append(std::move(exponent_node));
		    derivative->append(std::move(power_node));
		    
		    derivative->append(this->exponent());
		    derivative->append(std::move(base_prime));
		    return std::move(derivative);
		}
		else if (base_prime->is_zero()) {
		    derivative->append_clone(*this);
		    function::ptr function_node = std::make_unique<function>("Ln");
		    function_node->append(this->base());
		    derivative->append(std::move(function_node));
		    derivative->append(std::move(expo_prime));
		    return std::move(derivative);
		}
		else /* Both are dependent on var */{
		    derivative->append_clone(*this);
		    
		    add::ptr sub_parts = std::make_unique<add>();
		    
		    multiply::ptr lhs = std::make_unique<multiply>();
		    lhs->append(this->exponent());
		    lhs->append(std::move(base_prime));
		    lhs->append(inverse::make_inverse(*(this->base())));
		    sub_parts->append(std::move(lhs));
		    
		    multiply::ptr rhs = std::make_unique<multiply>();
		    function::ptr logarithm = std::make_unique<function>("Ln");
		    logarithm->append(this->base());
		    rhs->append(std::move(logarithm));
		    rhs->append(std::move(expo_prime));
		    sub_parts->append(std::move(rhs));
		    derivative->append(std::move(sub_parts));
		    return std::move(derivative);
		}
	    }

	    base_node::ptr function::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw invalid_node("Functioned node is undefined");
		}
		
		if (this->size() != 1) {
		    throw std::invalid_argument("Not implemented yet");
		}
		
		if (built_in_derivatives.find(this->get_token()) == built_in_derivatives.end()) {
		    throw std::invalid_argument(std::string("Derivative not implemented for ").append(get_token()));
		}

		base_node::ptr derivative = base_node::ptr(built_in_derivatives.at(this->get_token())->clone());
		
		std::map<std::string, base_node::ptr> change;
		change.insert(std::pair<std::string, base_node::ptr>("__", base_node::ptr(this->children[0]->clone())));
		
		derivative = derivative->change_variables(change);
		
		base_node::ptr chain_derivative = this->children[0]->derivative(var);
		if ( chain_derivative->is_zero() || derivative->is_zero() ) {
		    return std::make_unique<integer>(0);
		}
		else if ( chain_derivative->is_one() ) {
		    return derivative;
		}
		else if (derivative->is_one() ) {
		    return chain_derivative;
		}
		else {
		    if (derivative->get_token() == "*" && chain_derivative->get_token() == "*") {
			auto temp_ptr = static_cast<operation*>(derivative.release());
			operation::ptr deriv_handle = operation::ptr(temp_ptr);
			
			temp_ptr = static_cast<operation*>(chain_derivative.release());
			operation::ptr chain_handle = operation::ptr(temp_ptr);
			
			deriv_handle->getChildren().insert(deriv_handle->getChildren().end(),
							   std::make_move_iterator(chain_handle->getChildren().begin()),
							   std::make_move_iterator(chain_handle->getChildren().end())
						    );
			return std::move(deriv_handle);
		    }
		    else if (derivative->get_token() == "*") {
			auto temp_ptr = static_cast<operation*>(derivative.release());
			operation::ptr deriv_handle = operation::ptr(temp_ptr);
			deriv_handle->append(std::move(chain_derivative));
			return std::move(deriv_handle);
		    }
		    else if (chain_derivative->get_token() == "*") {
			auto temp_ptr = static_cast<operation*>(chain_derivative.release());
			operation::ptr chain_handle = operation::ptr(temp_ptr);
			chain_handle->getChildren().insert(chain_handle->getChildren().begin(), std::move(derivative) );
			return std::move(chain_handle);
		    }
		    else {
			multiply::ptr multiply_node = std::make_unique<multiply>();
			multiply_node->append(std::move(derivative));
			multiply_node->append(std::move(chain_derivative));
			return std::move(multiply_node);
		    }
		}
	    }

	    /*********************************************************
	     * Changing of variables
	     ********************************************************/
	    base_node::ptr add::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		add::ptr result = add::ptr(this->clone());
		for (auto iter = result->begin(); iter != result->end(); iter++) {
		    if ( !(*iter)->is_leaf()) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			base_node::ptr replacement = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			if (replacement->get_token() == "+") {
			    add* replacement_ptr = static_cast<add*>(replacement.get());
			    iter = result->getChildren().erase(iter);
			    std::size_t insertion_length = replacement_ptr->size();
			    iter = result->getChildren().insert(iter, 
								std::make_move_iterator(replacement_ptr->begin()), 
								std::make_move_iterator(replacement_ptr->end())
			    );
			    iter += insertion_length -1;
			}
			else {
			    (*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			}
		    }
		}
		return std::move(result);
	    }

	    base_node::ptr multiply::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		multiply::ptr result = multiply::ptr(this->clone());
		for (auto iter = result->begin(); iter != result->end(); iter++) {
		    if ( (*iter)->size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			base_node::ptr replacement = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			if (replacement->get_token() == "*") {
			    multiply* replacement_ptr = static_cast<multiply*>(replacement.get());
			    iter = result->getChildren().erase(iter);
			    std::size_t insertion_length = replacement_ptr->size();
			    iter = result->getChildren().insert(iter, 
						    std::make_move_iterator(replacement_ptr->begin()), 
						    std::make_move_iterator(replacement_ptr->end())
			    );
			    iter += insertion_length -1;
			}
			else {
			    (*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			}
		    }
		}
		return std::move(result);
	    }
	    
	    base_node::ptr power::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		power::ptr result = power::ptr(this->clone());
		for (auto iter = result->begin(); iter != result->end(); iter++) {
		    if ( (*iter)->size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			(*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
		    }
		}
		return std::move(result);
	    }

	    base_node::ptr function::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		function::ptr result = function::ptr(this->clone());
		for (auto iter = result->begin(); iter != result->end(); iter++) {
		    if ( (*iter)->size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			(*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
		    }
		}
		return std::move(result);
	    }

	    base_node::ptr variable::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		base_node::ptr result = base_node::ptr(this->clone());
		if ( changes.find(result->get_token()) != changes.end()) {
		    result = base_node::ptr( changes.at(result->get_token())->clone() );
		}
		return result;
	    }

	    base_node::ptr make_leaf_node(const std::string& str) {
		if (utils::find_lowest_priority(str) != -1) {
		    throw std::invalid_argument(std::string("Cannot make a leaf node from given string: ").append(str));
		}
		node::base_node::ptr leaf;
		if (utils::is_string_integer(str)) {
		    leaf = std::make_unique<node::integer>(str);
		}
		else if (utils::is_string_num(str)) {
		    leaf = std::make_unique<node::number>(str);
		}
		else {
		    leaf = std::make_unique<node::variable>(str);
		}
		return leaf;
	    }

	    base_node::ptr make_function_node(const std::string& str) {
		if (utils::find_lowest_priority(str) != -2) {
		    throw std::invalid_argument(std::string("Cannot make a function node from given string: ").append(str));
		}
		std::string newStr( str.substr(str.find("[")+1, str.size()-2-str.find("[")) );
		std::string funcName(str.substr(0,str.find("[")));

		std::vector<std::string> subStrings;
		boost::split(subStrings, newStr, boost::is_any_of(","));
		base_node::vec subExpressions;
		subExpressions.reserve(subStrings.size());

		for (auto iter = subStrings.begin(); iter != subStrings.end(); iter++) {
		    subExpressions.push_back(make_tree(*iter));
		}
		return std::make_unique<function>(funcName, subExpressions);
	    }

	    base_node::ptr make_tree(std::string str) {
		str = utils::remove_enclosing_parens(str);
		if (utils::is_string_num(str)) {
		    return make_leaf_node(str);
		}
		else if (utils::is_function(str)) {
		    return make_function_node(str);
		}
		int index = utils::find_lowest_priority(str);
		if ( index == -1) {
		    if (str[0] == '-') {
			str = str.substr(1, str.size()-1);
			str = std::string("(-1)*(").append(str).append(")");
			index = 4;
		    }
		    else {
			return make_leaf_node(str);
		    }
		}	   
		std::string operation(1, str[index]);		
		base_node::ptr tree;
		if (operation == "+" || 
		    operation == "-" || 
		    operation == "*" || 
		    operation == "/") {
		    std::vector<std::string> subStrings;
		    utils::break_string(str, operation, subStrings);
		    
		    base_node::vec subTrees;
		    if (subStrings.size() == 2 && operation == "-" && subStrings[0] == "0") {
			subTrees.push_back(std::make_unique<integer>(-1));
			subTrees.push_back(make_tree(subStrings[1]));
			tree = std::make_unique<multiply>(subTrees );
		    }
		    else if (subStrings.size() == 2 && operation == "/" && subStrings[0] == "1") {
			tree = make_tree(subStrings[1]);
		    }
		    else {
			subTrees.reserve(subStrings.size());
			for (auto iter = subStrings.begin(); iter != subStrings.end(); iter++) {
			    subTrees.push_back(make_tree(*iter));
			}
			if (operation == "+" || operation == "-") {
			    tree = std::make_unique<add>(subTrees);
			}
			else {
			    tree = std::make_unique<multiply>(subTrees);
			}
		    }
		}
		// Power node
		else {
		    std::string substr1(str.substr(0,index));
		    std::string substr2(str.substr(index+1,str.size()-index));
		    tree = std::make_unique<power>(*make_tree(substr1), *make_tree(substr2));
		}
		return tree;
	    }

	    std::map<std::string, base_node::ptr> _built_in_derivatives() {
		std::map<std::string, base_node::ptr> map;
		map.insert( std::pair<std::string, base_node::ptr>("Sin", std::move(make_tree("Cos[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Cos", std::move(make_tree("-1*Sin[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Tan", std::move(make_tree("Sec[__]^(2)"))));
		map.insert( std::pair<std::string, base_node::ptr>("Csc", std::move(make_tree("-1*Cot[__]*Csc[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Sec", std::move(make_tree("Sec[__]*Tan[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Cot", std::move(make_tree("-1*Csc[__]^(2)"))));
		map.insert( std::pair<std::string, base_node::ptr>("ArcSin", std::move(make_tree("1/((1-__^(2))^(1/2))"))));
		map.insert( std::pair<std::string, base_node::ptr>("ArcCos", std::move(make_tree("-1/((1-__^(2))^(1/2))"))));
		map.insert( std::pair<std::string, base_node::ptr>("ArcTan", std::move(make_tree("1/(1+__^(2))"))));
		map.insert( std::pair<std::string, base_node::ptr>("Sinh", std::move(make_tree("Cosh[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Cosh", std::move(make_tree("Sinh[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Tanh", std::move(make_tree("Sech[__]^(2)"))));
		map.insert( std::pair<std::string, base_node::ptr>("Csch", std::move(make_tree("-1*Coth[__]*Csch[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Sech", std::move(make_tree("-1*Sech[__]*Tanh[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Coth", std::move(make_tree("-1*Csch[__]^(2)"))));
		map.insert( std::pair<std::string, base_node::ptr>("ArcSinh", std::move(make_tree("1/((+-__^(2))^(1/2))"))));
		map.insert( std::pair<std::string, base_node::ptr>("ArcCosh", std::move(make_tree("1/((__-1)^(1/2)*(__+1)^(1/2))"))));
		map.insert( std::pair<std::string, base_node::ptr>("ArcTanh", std::move(make_tree("1/(1-x^(2))"))));
		map.insert( std::pair<std::string, base_node::ptr>("Exp", std::move(make_tree("Exp[__]"))));
		map.insert( std::pair<std::string, base_node::ptr>("Ln", std::move(make_tree("1/__"))));
		map.insert( std::pair<std::string, base_node::ptr>("Log", std::move(make_tree("1/(__*Ln[10])"))));
		return map;
	    }

	    const std::map<std::string, base_node::ptr> built_in_derivatives = std::move(_built_in_derivatives());
	}
    }
}

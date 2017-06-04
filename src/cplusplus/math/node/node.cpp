#include "Gold/math/node.hpp"
#include "Gold/math/utils.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cmath>
#include <iostream>
#include <iterator>

namespace Gold {
    namespace math {
	namespace node {

	    base_node::base_node(const base_node::vec& _children) {
		this->children.reserve(_children.size());
		for (auto iter = _children.cbegin(); iter != _children.cend(); iter++) {
		    this->children.push_back( std::unique_ptr<base_node>( (*iter)->clone() ) );
		}
	    }

	    base_node::base_node(base_node::vec&& _children) {
		this->children = std::move(_children);
	    }

	    base_node::base_node(const base_node& other) {
		this->children.reserve(other.children.size());
		for (auto iter = other.children.cbegin(); iter != other.children.cend(); iter++) {
		    base_node::ptr ptr = std::unique_ptr<base_node>((*iter)->clone());
		    this->children.push_back(std::move(ptr));
		}
	    }

	    base_node::base_node(base_node&& other) {
		this->children = std::move(other.children);
	    }

	    base_node& base_node::operator=(const base_node& other) {
		if (&other != this) {
		    this->children.clear();
		    for (auto iter = other.children.begin(); iter != other.children.end(); iter++) {
			base_node::ptr ptr = std::unique_ptr<base_node>( (*iter)->clone() );
			this->children.push_back(std::move(ptr));
		    }
		}
		return *this;
	    }
	    
	    base_node& base_node::operator=(base_node&& other) {
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
		inverse_node->children.push_back( base_node::ptr(denominator.clone()) );
		inverse_node->children.push_back( std::make_unique<integer>(-1) );
		return inverse_node;
	    }

	    base_node::ptr inverse::denominator() const {
		if (children.size() != 2) {
		    throw std::string("inverse node not initialized correctly");
		}
		return base_node::ptr(children[0]->clone());
	    }
	    
	    quotient::quotient(const base_node& numerator, const base_node& denominator) {
		children.clear();
		children.reserve(2);
		children.push_back( base_node::ptr(numerator.clone()) );
		children.push_back( inverse::make_inverse(denominator) );
	    }

	    base_node::ptr quotient::numerator() const { 
		if (children.size() != 2) {
		    throw "Quotient node not initialized correctly";
		}
		return base_node::ptr( children[0]->clone() ); 
	    }
	    
	    base_node::ptr quotient::denominator() const { 
		if (children.size() != 2 || children[1]->children.size() != 2) {
		    throw "Quotient node not initialized correctly";
		}
		return base_node::ptr( children[1]->children[0]->clone() );
	    }
	    
	    rational::rational(int numerator, int denominator) {
		    children.clear();
		    children.reserve(2);
		    children.push_back(std::make_unique<integer>(numerator));
		    children.push_back(inverse::make_inverse( integer(denominator) ) );
	    }

	    /*************************************************
	     *
	     * Definitions of evaluate
	     *
	     ************************************************/
	    double add::evaluate(const std::map<std::string, double>& args) const {
		if (children.empty()) {
			throw std::string("Add node not initialized with children");
		}
		double sum = 0;
		for (auto iter = children.begin(); iter != children.end(); iter++) {
		    sum += (*iter)->evaluate(args);
		}
		return sum;
	    }

	    double multiply::evaluate(const std::map<std::string, double>& args) const {
		if (children.empty()) {
			throw std::string("multiply node not initialized with children");
		}
		double product = 1;
		for (auto iter = children.begin(); iter != children.end(); iter++) {
		    product *= (*iter)->evaluate(args);
		}
		return product;
	    }	
	    
	    power::power(const base_node& base, const base_node& exponent) {
		children.reserve(2);
		children.push_back( std::unique_ptr<base_node>(base.clone()));
		children.push_back( std::unique_ptr<base_node>(exponent.clone()));
	    }

	    base_node::ptr power::base() const { 
		if (children.empty()) {
		    throw std::string("Power node not initialized with children");
		}
		return base_node::ptr( children[0]->clone() ); 
	    }

	    base_node::ptr power::exponent() const {
		if (children.empty()) {
		    throw std::string("Power node not initialized with children");
		}
		return base_node::ptr( children[1]->clone() ); 
	    }

	    double power::evaluate(const std::map<std::string, double>& args) const {
		if (children.size() != 2) {
			throw std::string("Power node initialized with more or less than two children");
		}
		double base = this->base()->evaluate(args);
		double expo = this->exponent()->evaluate(args);	
		return std::pow(base, expo);
	    }

	    double function::evaluate(const std::map<std::string, double>& args) const {
		if (children.empty()) {
			throw std::string("Function node not initialized with children");
		}
		if ( built_in_functions.find(get_token()) != built_in_functions.end()) {
		    if (children.size() != 1) {
			throw std::string("Built in functions take only one argument");
		    }
		    else {
			double value = this->children[0]->evaluate(args);
			return built_in_functions.at(get_token())(value);
		    }
		}
		else {
		    throw std::string(get_token()).append(" not found in functions");
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
		    throw std::string("Variable not found\n");
		}
		return iter->second;
	    }

	    /******************************************************
	     * Build string operations
	     *****************************************************/
	    std::string add::string() const {
		if (children.size() == 0) {
		    return "";
		}
		std::string token = this->get_token();
		std::string first = children[0]->string();
		for (auto iter = children.begin()+1; iter != children.end(); iter++) {
		    Gold::math::utils::append_with_parens(first, (*iter)->string(), token, true);
		}
		return first;
	    }
	    
	    std::string multiply::string() const {
		if (children.size() == 0) {
		    return "";
		}
		std::string token = this->get_token();
		std::string first = children[0]->string();
		if (children[0]->get_token() == "+") {
		    first = std::string("(").append(first).append(")");
		}
		for (auto iter = children.begin()+1; iter != children.end(); iter++) {
		    Gold::math::utils::append_with_parens(first, (*iter)->string(), token, true);
		}
		return first;
	    }

	    std::string power::string() const {
		if (children.size() == 0) {
		    return "";
		}
		std::string token = this->get_token();
		std::string first = children[0]->string();
		if (children[0]->get_token() == "+" || children[0]->get_token() == "*" || children[0]->get_token() == "^") {
		    first = std::string("(").append(first).append(")");
		}
		for (auto iter = children.begin()+1; iter != children.end(); iter++) {
		    Gold::math::utils::append_with_parens(first, (*iter)->string(), token, true);
		}
		return first;
	    }

	    std::string function::string() const {
		if (children.size() == 0) {
		    return "";
		}
		
		std::string str = get_token().append("[");
		std::vector<std::string> subStrings;
		subStrings.reserve(children.size());
		for (auto iter = children.begin(); iter != children.end(); iter++) {
		    subStrings.push_back( (*iter)->string() );
		}
		str.append( boost::algorithm::join(subStrings, ", ") ).append("]");
		return str;
	    }

	    base_node::ptr add::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw std::string("Node is undefined, cannot take derivative");
		}
		base_node::vec derivative_vector;
		derivative_vector.reserve(children.size());
		for (auto iter = children.begin(); iter != children.end(); iter++) {
		    auto derivative_node = (*iter)->derivative(var);
		    if (!derivative_node->is_zero()) {
			derivative_vector.push_back(std::move(derivative_node) );
		    }
		}
		return std::make_unique<add>(derivative_vector);
	    }

	    base_node::ptr multiply::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw std::string("Node is undefined, cannot take derivative");
		}
		if (is_zero() || is_one() || is_minus_one()) {
		    return std::make_unique<integer>(0);
		}
		base_node::vec derivative_vector;
		for (auto iter = children.begin(); iter != children.end(); iter++) {
		    base_node::vec sub_vector;
		    bool is_derivative_zero = false;
		    for (auto jter = children.begin(); jter != children.end(); jter++) {
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
		    throw std::string("Power node is undefined");
		}
		if (is_zero() || is_one() || is_minus_one()) {
		    return std::make_unique<integer>(0);
		}
		
		base_node::ptr base_prime = this->base()->derivative(var);
		base_node::ptr expo_prime = this->exponent()->derivative(var);
		base_node::ptr derivative = std::make_unique<multiply>();
		if (base_prime->is_zero() && expo_prime->is_zero()) {
		    return std::make_unique<integer>(0);
		}
		else if (expo_prime->is_zero()) {
		    power::ptr power_node = std::make_unique<power>();
		    power_node->children.push_back(this->base());
		    add::ptr exponent_node = std::make_unique<add>();
		    exponent_node->children.push_back(this->exponent());
		    exponent_node->children.push_back(std::make_unique<integer>(-1));
		    power_node->children.push_back(std::move(exponent_node));
		    derivative->children.push_back(std::move(power_node));
		    
		    derivative->children.push_back(this->exponent());
		    derivative->children.push_back(std::move(base_prime));
		    return derivative;
		}
		else if (base_prime->is_zero()) {
		    derivative->children.push_back(power::ptr(this->clone()));
		    function::ptr function_node = std::make_unique<function>("Ln");
		    function_node->children.push_back(this->base());
		    derivative->children.push_back(std::move(function_node));
		    derivative->children.push_back(std::move(expo_prime));
		    return derivative;
		}
		else /* Both are dependent on var */{
		    derivative->children.push_back(power::ptr(this->clone()));
		    
		    add::ptr sub_parts = std::make_unique<add>();
		    
		    multiply::ptr lhs = std::make_unique<multiply>();
		    lhs->children.push_back(this->exponent());
		    lhs->children.push_back(std::move(base_prime));
		    lhs->children.push_back(inverse::make_inverse(*(this->base())));
		    sub_parts->children.push_back(std::move(lhs));
		    
		    multiply::ptr rhs = std::make_unique<multiply>();
		    function::ptr logarithm = std::make_unique<function>("Ln");
		    logarithm->children.push_back(this->base());
		    rhs->children.push_back(std::move(logarithm));
		    rhs->children.push_back(std::move(expo_prime));
		    sub_parts->children.push_back(std::move(rhs));
		    derivative->children.push_back(std::move(sub_parts));
		    return derivative;
		}
	    }

	    base_node::ptr function::derivative(const std::string& var) const {
		if (is_undefined()) {
		    throw std::string("Functioned node is undefined");
		}
		
		if (this->children.size() != 1) {
		    throw std::string("Not implemented yet");
		}
		
		if (built_in_derivatives.find(this->get_token()) == built_in_derivatives.end()) {
		    throw std::string("Derivative not implemented for ").append(get_token());
		}

		base_node::ptr derivative = base_node::ptr(built_in_derivatives.at(this->get_token())->clone());
		
		std::map<std::string, base_node::ptr> change;
		change.insert(std::pair<std::string, base_node::ptr>("__", base_node::ptr(this->children[0]->clone())));
		
		derivative = derivative->change_variables(change);
		
		base_node::ptr chain_derivative = this->children[0]->derivative(var);
		if ( chain_derivative->is_zero() ) {
		    return std::make_unique<integer>(0);
		}
		else if ( chain_derivative->is_one() ) {
		    return derivative;
		}
		else {
		    if (derivative->get_token() == "*" && chain_derivative->get_token() == "*") {
			derivative->children.insert(derivative->children.end(),
						    std::make_move_iterator(chain_derivative->children.begin()),
						    std::make_move_iterator(chain_derivative->children.end())
						    );
			return derivative;
		    }
		    else if (derivative->get_token() == "*") {
			derivative->children.push_back(std::move(chain_derivative));
			return derivative;
		    }
		    else if (chain_derivative->get_token() == "*") {
			chain_derivative->children.insert(chain_derivative->children.begin(), std::move(derivative) );
			return chain_derivative;
		    }
		    else {
			base_node::ptr multiply_node = std::make_unique<multiply>();
			multiply_node->children.push_back(std::move(derivative));
			multiply_node->children.push_back(std::move(chain_derivative));
			return multiply_node;
		    }
		}
	    }

	    /*********************************************************
	     * Changing of variables
	     ********************************************************/
	    base_node::ptr add::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		base_node::ptr result = base_node::ptr(this->clone());
		for (auto iter = result->children.begin(); iter != result->children.end(); iter++) {
		    if ( (*iter)->children.size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			base_node::ptr replacement = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			if (replacement->get_token() == "+") {
			    iter = result->children.erase(iter);
			    std::size_t insertion_length = replacement->children.size();
			    iter = result->children.insert(iter, 
						    std::make_move_iterator(replacement->children.begin()), 
						    std::make_move_iterator(replacement->children.end())
			    );
			    iter += insertion_length -1;
			}
			else {
			    (*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			}
		    }
		}
		return result;
	    }

	    base_node::ptr multiply::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		base_node::ptr result = base_node::ptr(this->clone());
		for (auto iter = result->children.begin(); iter != result->children.end(); iter++) {
		    if ( (*iter)->children.size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			base_node::ptr replacement = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			if (replacement->get_token() == "*") {
			    iter = result->children.erase(iter);
			    std::size_t insertion_length = replacement->children.size();
			    iter = result->children.insert(iter, 
						    std::make_move_iterator(replacement->children.begin()), 
						    std::make_move_iterator(replacement->children.end())
			    );
			    iter += insertion_length -1;
			}
			else {
			    (*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
			}
		    }
		}
		return result;
	    }
	    
	    base_node::ptr power::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		base_node::ptr result = base_node::ptr(this->clone());
		for (auto iter = result->children.begin(); iter != result->children.end(); iter++) {
		    if ( (*iter)->children.size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			(*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
		    }
		}
		return result;
	    }

	    base_node::ptr function::change_variables(const std::map<std::string, base_node::ptr>& changes) const {
		base_node::ptr result = base_node::ptr(this->clone());
		for (auto iter = result->children.begin(); iter != result->children.end(); iter++) {
		    if ( (*iter)->children.size() != 0 ) {
			(*iter) = (*iter)->change_variables(changes);
		    }
		    else if ( changes.find((*iter)->get_token()) != changes.end() ) {
			(*iter) = base_node::ptr( changes.at((*iter)->get_token())->clone() );
		    }
		}
		return result;
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
		    throw std::string("Cannot make a leaf node from given string: ").append(str);
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
		    throw std::string("Cannot make a function node from given string: ").append(str);
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

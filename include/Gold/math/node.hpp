#ifndef GOLD_MATH_NODE
#define GOLD_MATH_NODE

#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <map>
#include <iostream>

namespace Gold {
    namespace math {
	namespace node {
	    
	    class operation;
	    class add;
	    class multiply;
	    class power;
	    class function;
	    class inverse;
	    class quotient;

	    class integer;
	    class number;
	    class variable;
	    
	    class base_node {
	    public:
		typedef std::unique_ptr<base_node> ptr;
		typedef std::vector<ptr> vec;
		base_node() { //Intentionally empty
		}
		virtual base_node* clone() const=0;
		virtual ~base_node() { //Intentionally empty
		}
		virtual uint size() const { return 0; }
		virtual std::string get_token() const=0;
		virtual bool is_zero() const { return false; }
		virtual bool is_one() const { return false; }
		virtual bool is_minus_one() const { return false; }
		virtual bool is_undefined() const=0;
		virtual bool is_leaf() const { return true; }
		
		virtual double evaluate(const std::map<std::string, double>& args = { }) const=0;
		
		virtual ptr numerator() const;
		virtual ptr denominator() const;
		virtual ptr base() const;
		virtual ptr exponent() const;
		virtual std::string string() const=0;
		virtual ptr derivative(const std::string& var) const=0;
		virtual ptr change_variables(const std::map<std::string, ptr>& changes) const=0;
	    };
	    
	    class operation : public base_node {
	    public:
		typedef std::unique_ptr<operation> ptr;
		typedef base_node::vec::iterator iterator;
		typedef base_node::vec::const_iterator const_iterator;
		
		operation() { //Intentionally empty
		}
		explicit operation(const base_node::vec& _children);
		explicit operation(base_node::vec&& _children);
		explicit operation(const operation& other);
		explicit operation(operation&& other);
		operation& operator=(const operation& other);
		operation& operator=(operation&& other);
		
		iterator begin() { return children.begin(); }
		const_iterator begin() const { return children.begin(); }
		iterator end() { return children.end(); }
		const_iterator end() const { return children.end(); }
		base_node& child(uint i) const { 
		    if (i >= size()) {
			throw std::length_error("Out of bounds");
		    }
		    return *(children[i]);
		}
		
		void append(base_node::ptr&& node) {
		    children.push_back(std::move(node));
		} 
		void append_clone(const base_node& new_child) {
		    children.push_back( std::unique_ptr<base_node>(new_child.clone()) );
		}
		
		//TODO Remove this function and any references to it and replace with specialized functions
		base_node::vec& getChildren() { return children; }
		virtual uint size() const { return children.size(); }
		virtual bool is_leaf() const { return children.empty(); }
		virtual bool is_undefined() const { 
		    return size() == 0 ||
			std::any_of(children.begin(), children.end(), [](const base_node::ptr& iter) {
				return iter->is_undefined();
			    });    
		}
	    protected:
		base_node::vec children;
	    };

	    class add : public operation {
	    public:
		typedef std::unique_ptr<add> ptr;
		add() { //Intentionally empty
		}
		explicit add(const base_node::vec& _children) : operation(_children) { }
		explicit add(base_node::vec&& _children) : operation(_children) { }
		explicit add(const add& other) : operation(other) { }
		explicit add(add&& other) : operation(other) { }
		add& operator=(const add& other) { operation::operator=(other); return *this;}
		add& operator=(add&& other) { operation::operator=(other); return *this;}
		virtual add* clone() const { return new add(*this); }
		virtual ~add() { //Intentionally empty
		}
		virtual std::string get_token() const { return "+"; }
		virtual double evaluate(const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };
	    
	    class multiply : public operation {
	    public:
		typedef std::unique_ptr<multiply> ptr;
		multiply() { //Intentionally empty
		}
		explicit multiply(const base_node::vec& _children) : operation(_children) { }
		explicit multiply(base_node::vec&& _children) : operation(_children) { }
		explicit multiply(const multiply& other) : operation(other) { }
		explicit multiply(multiply&& other) : operation(other) { }
		multiply& operator=(const multiply& other) { operation::operator=(other); return *this;}
		multiply& operator=(multiply&& other) { operation::operator=(other); return *this;}
		virtual multiply* clone() const { return new multiply(*this); }
		virtual ~multiply() { //Intentionally empty
		}
		virtual bool is_zero() const {
		    return std::any_of(children.begin(), children.end(), [](const base_node::ptr& iter) {
			    return iter->is_zero();
			});
		}
		virtual bool is_one() const {
		    int negative_one_count = 0;
		    for (auto iter = children.begin(); iter != children.end(); iter++) {
			if ( (*iter)->is_minus_one() ) {
			    negative_one_count++;
			}
			else if ( !(*iter)->is_one() ) {
			    return false;
			}
		    }
		    return (negative_one_count & 1) == 0;
		}
		virtual bool is_minus_one() const {
		    int negative_one_count = 0;
		    for (auto iter = children.begin(); iter != children.end(); iter++) {
			if ( (*iter)->is_minus_one() ) {
			    negative_one_count++;
			}
			else if ( !(*iter)->is_one() ) {
			    return false;
			}
		    }
		    return (negative_one_count & 1) == 1;
		}    
		virtual std::string get_token() const { return "*"; }
		virtual double evaluate(const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };
	    
	    class power : public operation {
	    public:
		typedef std::unique_ptr<power> ptr;
		power() { //Intentionally empty
		}
		explicit power(const base_node& base, const base_node& exponent);
		explicit power(const power& other) : operation(other) { }
		explicit power(power&& other) : operation(other) { }
		power& operator=(const power& other) { operation::operator=(other); return *this;}
		power& operator=(power&& other) { operation::operator=(other); return *this;}
		virtual power* clone() const { return new power(*this); }
		virtual ~power() { //Intentionally empty
		}
		virtual bool is_zero() const { return this->base()->is_zero(); }
		virtual bool is_one() const { 
		    auto base = this->base();
		    auto expo = this->exponent();
		    return (expo->is_zero() && !base->is_zero()) || base->is_one();
		}
		virtual bool is_minus_one() const {
		    auto exponent = this->exponent();
		    return this->base()->is_minus_one() && ( exponent->is_one() || exponent->is_minus_one() );
		}
		virtual bool is_undefined() const { 
		    auto exponent = this->exponent();
		    return operation::is_undefined() || 
			( this->base()->is_zero() && (exponent->is_zero() || exponent->is_minus_one())) ;
		}
		virtual std::string get_token() const { return "^"; }
		virtual base_node::ptr base() const;
		virtual base_node::ptr exponent() const; 
		virtual double evaluate(const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };
	    
	    class function : public operation {
	    private:
		std::string token;
	    public:
		typedef std::unique_ptr<function> ptr;
		function() { //Intentionally empty
		}
		function(const std::string& _token, const base_node::vec& _children={}) : operation(_children), token(_token) { }
		function(const std::string& _token, base_node::vec&& _children) : operation(_children), token(_token) { }
		function(const function& other) : operation(other) {token = other.token; }
		function(function&& other) : operation(other) { token = other.token; }
		function& operator=(const function& other) { 
		    operation::operator=(other);
		    token = other.token;
		    return *this;
		}
		function& operator=(function&& other) { 
		    operation::operator=(other); 
		    token = other.token;
		    return *this;
		}
		virtual function* clone() const { return new function(*this); }
		virtual ~function() { //Intentionally empty
		}
		virtual std::string get_token() const { return token; }
		virtual double evaluate(const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };
	    
	    class integer : public base_node {
	    private:
		int token;
	    public:
		typedef std::unique_ptr<integer> ptr;
		integer() { //Intentionally empty
		}
		explicit integer(const std::string& _token) : token(std::stoi(_token)) { }
		explicit integer(int _token) : token(_token) { }
		virtual integer* clone() const { return new integer(*this); }
		virtual bool is_zero() const{ return token == 0; }
		virtual bool is_one() const { return token == 1; }
		virtual bool is_minus_one() const { return token == -1; }
		virtual bool is_undefined() const { return false; }
		virtual std::string get_token() const { return std::to_string(token); }
		virtual double evaluate(const std::map<std::string, double>& /*args*/ = { }) const;
		virtual std::string string() const { return this->get_token(); }
		virtual base_node::ptr derivative(const std::string& var) const { return std::make_unique<integer>(0); }
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const { 
		    return base_node::ptr(this->clone()); 
		}
	    };

	    class number : public base_node {
	    private:
		double token;
	    public:
		typedef std::unique_ptr<number> ptr;
		number() { //Intentionally empty
		}
		explicit number(const std::string& _token) : token(std::stof(_token)) { }
		explicit number(double _token = 0) : token(_token) { }
		virtual number* clone() const { return new number(*this); }
		virtual bool is_zero() const { return token == 0; }
		virtual bool is_one() const { return token == 1; }
		virtual bool is_minus_one() const { return token == -1; }
		virtual bool is_undefined() const { return false; }
		virtual std::string get_token() const { return std::to_string(token); }
		virtual double evaluate(const std::map<std::string, double>& /*args*/ = { }) const;
		virtual std::string string() const { return this->get_token(); }
		virtual base_node::ptr derivative(const std::string& var) const { return std::make_unique<integer>(0); }
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const { 
		    return base_node::ptr(this->clone()); 
		}
	    };

	    class variable : public base_node {
	    private:
		std::string token;
	    public:
		typedef std::unique_ptr<variable> ptr;
		variable() { //Intentionally empty
		}
		explicit variable(const std::string& _token) : token(_token) { }
		virtual variable* clone() const { return new variable(*this); }
		virtual bool is_undefined() const { return false; }
		virtual std::string get_token() const { return token; }
		virtual double evaluate(const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const { return this->get_token(); }
		virtual base_node::ptr derivative(const std::string& var) const { 
		    if (get_token() == var) { 
			return std::make_unique<integer>(1);
		    }
		    else {
			return std::make_unique<integer>(0);
		    }
		}
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };

	    class inverse : public power {
	    public:
		typedef std::unique_ptr<inverse> ptr;
		inverse() { //Intentionally empty
		}
		inverse(const inverse& other) : power(other) { }
		inverse(inverse&& other) : power(other) { }
		inverse& operator=(const inverse& other) { 
		    power::operator=(other);
		    return *this;
		}
		inverse& operator=(inverse&& other) { 
		    power::operator=(other); 
		    return *this;
		}
		virtual inverse* clone() const { return new inverse(*this); }
		virtual ~inverse() { //Intentionally empty
		}
		virtual bool is_one() const { return this->denominator()->is_one(); }
		virtual bool is_zero() const { return false; }
		virtual bool is_minus_one() const { return this->denominator()->is_minus_one(); }
		virtual bool is_undefined() const { return this->denominator()->is_zero(); }
		virtual base_node::ptr numerator() const { return std::make_unique<integer>(1); }
		virtual base_node::ptr denominator() const;
		static ptr make_inverse(const base_node& other);
	    };

	    class quotient : public multiply {
	    public:
		typedef std::unique_ptr<quotient> ptr;
		quotient() { //Intentionally empty
		}
		quotient(const base_node& numerator, const base_node& denominator);
		quotient(const quotient& other) : multiply(other) { }
		quotient(quotient&& other) : multiply(other) { }
		quotient& operator=(const quotient& other) { 
		    multiply::operator=(other);
		    return *this;
		}
		quotient& operator=(quotient&& other) { 
		    multiply::operator=(other); 
		    return *this;
		}
		virtual quotient* clone() const { return new quotient(*this); }
		virtual ~quotient() { //Intentionally empty
		}
		
		virtual bool is_one() {
		    auto numerator = this->numerator();
		    auto denominator = this->denominator();
		    return (denominator->is_one() && numerator->is_one()) || (denominator->is_minus_one() && numerator->is_minus_one());
		}
		
		virtual bool is_minus_one() { 
		    auto numerator = this->numerator();
		    auto denominator = this->denominator();
		    return (denominator->is_one() && numerator->is_minus_one()) || (denominator->is_minus_one() && numerator->is_one());
		}

		virtual bool is_undefined() { return this->denominator()->is_zero(); }
		virtual base_node::ptr numerator() const;
		virtual base_node::ptr denominator() const;
	    };

	    class rational : public quotient {
	    public:
		typedef std::unique_ptr<rational> ptr;
		rational() { //Intentonally empty
		}
		rational(const integer& numerator, const integer& denominator) : quotient(numerator, denominator) { }
		rational(int numerator, int denominator);
		rational(const rational& other) : quotient(other) { }
		rational(rational&& other) : quotient(other) { }
		virtual ~rational() { //Intentionally empty
		}
		rational& operator=(const rational& other) { 
		    quotient::operator=(other);
		    return *this;
		}
		rational& operator=(rational&& other) { 
		    quotient::operator=(other); 
		    return *this;
		}
		virtual rational* clone() const { return new rational(*this); }
		virtual base_node::ptr derivative(const std::string& var) const { return std::make_unique<integer>(0); }
	    };

	    base_node::ptr make_leaf_node(const std::string& str);
	    base_node::ptr make_function_node(const std::string& str);
	    base_node::ptr make_tree(std::string str);

	    const std::map<std::string, std::function<double(double)> > built_in_functions = {
		{"Sin", sin},
		{"Cos", cos},
		{"Tan", tan},
		{"Csc", [](double x) { return 1.0/sin(x); }},
		{"Sec", [](double x) { return 1.0/cos(x); }},
		{"Cot", [](double x) { return cos(x)/sin(x); }},
		{"Arcsin", asin},
		{"Arccos", acos},
		{"Arctan", atan},
		{"Sinh", sinh},
		{"Cosh", cosh},
		{"Tanh", tanh},
		{"Csch", [](double x) { return 1.0/sin(x); }},
		{"Sech", [](double x) { return 1.0/cos(x); }},
		{"Coth", [](double x) { return cosh(x)/sinh(x); }},
		{"Arcsinh", asinh},
		{"Arccosh", acosh},
		{"Arctanh", atanh},
		{"Exp", exp},
		{"Ln", log},
		{"Log", log10},
		{"Abs", abs},
		{"H", [](double x) { return (x < 0) ? 0.0 : 1.0; }}
	    };
	    
	    extern const std::map<std::string, base_node::ptr> built_in_derivatives;	

	}
    }
}

#endif       

#ifndef GOLD_MATH_NODE
#define GOLD_MATH_NODE

#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <map>

namespace Gold {
    namespace math {
	namespace node {

	    class add;
	    class multiply;
	    class power;
	    class function;
	    class integer;
	    class number;
	    class rational;

	    class base_node {
	    public:
		typedef std::unique_ptr<base_node> ptr;
		typedef std::vector<ptr> vec;
		base_node() { }
		base_node(const base_node::vec& _children);
		base_node(base_node::vec&& _children);
		base_node(const base_node& other);
		base_node(base_node&& other);
		base_node& operator=(const base_node& other);
		base_node& operator=(base_node&& other);
		virtual base_node* clone() const  { throw std::string("base_node::clone Should not be called"); }
		virtual ~base_node() { }
		virtual std::string get_token() const { throw std::string("base_node::token Should not be called"); }
		virtual bool is_zero() const { return false; }
		virtual bool is_one() const { return false; }
		virtual bool is_minus_one() const { return false; }
		
		virtual bool is_undefined() const { 
		    return children.size() == 0 ||
			std::any_of(children.begin(), children.end(), [](const base_node::ptr& iter) {
				return iter->is_undefined();
			    });    
		}
		
		virtual double operator()(const std::map<std::string, double>& args = { }) const { 
		    throw std::string("base_node::operator() Should not be called"); 
		}
		
		virtual ptr numerator() const;
		virtual ptr denominator() const;
		virtual ptr base() const;
		virtual ptr exponent() const;
		virtual std::string string() const { throw std::string("base_node::string Shouldn't be called"); }
		virtual ptr derivative(const std::string& var) const { throw std::string("basenode::derivative Shouldn't be called"); }
		virtual ptr change_variables(const std::map<std::string, ptr>& changes) const { throw std::string("base_node::change_variables Shouldn't be called"); }
		vec children;
	    };

	    class add : public base_node {
	    public:
		typedef std::unique_ptr<add> ptr;
		typedef std::unique_ptr<const add> c_ptr;
		add() { }
		add(const base_node::vec& _children) : base_node(_children) { }
		add(base_node::vec&& _children) : base_node(_children) { }
		add(const add& other) : base_node(other) { }
		add(add&& other) : base_node(other) { }
		add& operator=(const add& other) { base_node::operator=(other); return *this;}
		add& operator=(add&& other) { base_node::operator=(other); return *this;}
		virtual add* clone() const { return new add(*this); }
		virtual ~add() { }
		virtual std::string get_token() const { return "+"; }
		virtual double operator() (const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };

	    class multiply : public base_node {
	    public:
		typedef std::unique_ptr<multiply> ptr;
		typedef std::unique_ptr<const multiply> c_ptr;
		multiply() { }
		multiply(const base_node::vec& _children) : base_node(_children) { }
		multiply(base_node::vec&& _children) : base_node(_children) { }
		multiply(const multiply& other) : base_node(other) { }
		multiply(multiply&& other) : base_node(other) { }
		multiply& operator=(const multiply& other) { base_node::operator=(other); return *this;}
		multiply& operator=(multiply&& other) { base_node::operator=(other); return *this;}
		virtual multiply* clone() const { return new multiply(*this); }
		virtual ~multiply() { }
		virtual bool is_zero() const {
		    return std::any_of(children.begin(), children.end(), [](const base_node::ptr& iter) {
			    return iter->is_zero();
			});
		}
		virtual std::string get_token() const { return "*"; }
		virtual double operator() (const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };

	    class power : public base_node {
	    public:
		typedef std::unique_ptr<power> ptr;
		typedef std::unique_ptr<const power> c_ptr;
		power() { }
		power(const base_node& base, const base_node& exponent);
		power(const power& other) : base_node(other) { }
		power(power&& other) : base_node(other) { }
		power& operator=(const power& other) { base_node::operator=(other); return *this;}
		power& operator=(power&& other) { base_node::operator=(other); return *this;}
		virtual power* clone() const { return new power(*this); }
		virtual ~power() { }
		virtual bool is_zero() const { return this->base()->is_zero(); }
		virtual bool is_one() const { return this->exponent()->is_zero(); }
		virtual bool is_undefined() const { 
		    return base_node::is_undefined() || 
			( this->base()->is_zero() && this->exponent()->is_zero() ) ;
		}
		virtual std::string get_token() const { return "^"; }
		virtual base_node::ptr base() const;
		virtual base_node::ptr exponent() const; 
		virtual double operator() (const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };
	    
	    class function : public base_node {
	    private:
		std::string token;
	    public:
		typedef std::unique_ptr<function> ptr;
		typedef std::unique_ptr<const function> c_ptr;
		function() { }
		function(const std::string& _token, const base_node::vec& _children={}) : base_node(_children), token(_token) { }
		function(const std::string& _token, base_node::vec&& _children) : base_node(_children), token(_token) { }
		function(const function& other) : base_node(other) {token = other.token; }
		function(function&& other) : base_node(other) { token = other.token; }
		function& operator=(const function& other) { 
		    base_node::operator=(other);
		    token = other.token;
		    return *this;
		}
		function& operator=(function&& other) { 
		    base_node::operator=(other); 
		    token = other.token;
		    return *this;
		}
		virtual function* clone() const { return new function(*this); }
		virtual ~function() { }
		virtual std::string get_token() const { return token; }
		virtual double operator() (const std::map<std::string, double>& args = { }) const;
		virtual std::string string() const;
		virtual base_node::ptr derivative(const std::string& var) const;
		virtual base_node::ptr change_variables(const std::map<std::string, base_node::ptr>& changes) const;
	    };

	    class integer : public base_node {
	    private:
		int token;
	    public:
		typedef std::unique_ptr<integer> ptr;
		typedef std::unique_ptr<const integer> c_ptr;
		integer() { }
		explicit integer(const std::string& _token) : token(std::stoi(_token)) { }
		explicit integer(int _token) : token(_token) { }
		integer(const integer& other) : base_node(other) {token = other.token; }
		integer(integer&& other) : base_node(other) { token = other.token; }
		integer& operator=(const integer& other) { 
		    base_node::operator=(other);
		    token = other.token;
		    return *this;
		}
		integer& operator=(integer&& other) { 
		    base_node::operator=(other); 
		    token = other.token;
		    return *this;
		}
		virtual integer* clone() const { return new integer(*this); }
		virtual ~integer() { }
		virtual bool is_zero() const{ return token == 0; }
		virtual bool is_one() const { return token == 1; }
		virtual bool is_minus_one() const { return token == -1; }
		virtual bool is_undefined() const { return false; }
		virtual std::string get_token() const { return std::to_string(token); }
		virtual double operator() (const std::map<std::string, double>& /*args*/ = { }) const;
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
		typedef std::unique_ptr<const number> c_ptr;
		number() { }
		explicit number(const std::string& _token) : token(std::stof(_token)) { }
		explicit number(double _token = 0) : token(_token) { }
		number(const number& other) : base_node(other) {token = other.token; }
		number(number&& other) : base_node(other) { token = other.token; }
		number& operator=(const number& other) { 
		    base_node::operator=(other);
		    token = other.token;
		    return *this;
		}
		number& operator=(number&& other) { 
		    base_node::operator=(other); 
		    token = other.token;
		    return *this;
		}
		virtual number* clone() const { return new number(*this); }
		virtual ~number() { }
		virtual bool is_zero() const { return token == 0; }
		virtual bool is_one() const { return token == 1; }
		virtual bool is_minus_one() const { return token == -1; }
		virtual bool is_undefined() const { return false; }
		virtual std::string get_token() const { return std::to_string(token); }
		virtual double operator() (const std::map<std::string, double>& /*args*/ = { }) const;
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
		typedef std::unique_ptr<const variable> c_ptr;
		variable() { }
		variable(const std::string& _token) : token(_token) { }
		variable(const variable& other) : base_node(other) {token = other.token; }
		variable(variable&& other) : base_node(other) { token = other.token; }
		variable& operator=(const variable& other) { 
		    base_node::operator=(other);
		    token = other.token;
		    return *this;
		}
		variable& operator=(variable&& other) { 
		    base_node::operator=(other); 
		    token = other.token;
		    return *this;
		}
		virtual variable* clone() const { return new variable(*this); }
		virtual ~variable() { }
		virtual bool is_undefined() const { return false; }
		virtual std::string get_token() const { return token; }
		virtual double operator() (const std::map<std::string, double>& args = { }) const;
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
		typedef std::unique_ptr<const inverse> c_ptr;
		inverse() { }
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
		virtual ~inverse() { }
		virtual bool is_one() const { return this->denominator()->is_one(); }
		virtual bool is_minus_one() const { return this->denominator()->is_minus_one(); }
		virtual bool is_undefined() const { return this->denominator()->is_zero(); }
		virtual base_node::ptr numerator() const { return std::make_unique<integer>(1); }
		virtual base_node::ptr denominator() const;
		static ptr make_inverse(const base_node& other);
	    };

	    class quotient : public multiply {
	    public:
		typedef std::unique_ptr<quotient> ptr;
		typedef std::unique_ptr<const quotient> c_ptr;
		quotient() { }
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
		virtual ~quotient() { }
		
		virtual bool is_one() {
		    auto numerator = this->numerator();
		    auto denominator = this->denominator();
		    return (denominator->is_one() && numerator->is_one()) || (denominator->is_minus_one() && numerator->is_minus_one());
		}
		
		virtual bool is_minus_one() { 
		    auto numerator = this->numerator();
		    auto denominator = this->denominator();
		    return (denominator->is_one() && numerator->is_minus_one()) || (denominator->is_one() && numerator->is_minus_one());
		}

		virtual bool is_undefined() { return this->denominator()->is_zero(); }
		virtual base_node::ptr numerator() const;
		virtual base_node::ptr denominator() const;
	    };

	    class rational : public quotient {
	    public:
		typedef std::unique_ptr<rational> ptr;
		typedef std::unique_ptr<const rational> c_ptr;
		rational() { }
		rational(const integer& numerator, const integer& denominator) : quotient(numerator, denominator) { }
		rational(int numerator, int denominator);
		rational(const rational& other) : quotient(other) { }
		rational(rational&& other) : quotient(other) { }
		virtual ~rational() { }
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
		{"Csc", [](double x) -> double { return 1/sin(x); }},
		{"Sec", [](double x) -> double { return 1/cos(x); }},
		{"Cot", [](double x) -> double { return cos(x)/sin(x); }},
		{"Arcsin", asin},
		{"Arccos", acos},
		{"Arctan", atan},
		{"Sinh", sinh},
		{"Cosh", cosh},
		{"Tanh", tanh},
		{"Csch", [](double x) -> double { return 1/sin(x); }},
		{"Sech", [](double x) -> double { return 1/cos(x); }},
		{"Coth", [](double x) -> double { return cosh(x)/sinh(x); }},
		{"Arcsinh", asinh},
		{"Arccosh", acosh},
		{"Arctanh", atanh},
		{"Exp", exp},
		{"Ln", log},
		{"Log", log10},
		{"Abs", abs},
		{"H", [](double x) -> double { return (x < 0) ? 0 : 1; }}
	    };
	    
	    extern const std::map<std::string, base_node::ptr> built_in_derivatives;	

	}
    }
}

#endif       

/*********************************************
  This class is for turning a string into
  a usable function object. The string is treated
  as a mathematical expression, and is loaded into 
  a binary tree. The constructor is not exception safe.
  All calls to the object or the evaluate methods and
  the changeVariable(s) methods 
  offer the strong exception guarantee, and all other
  methods do no throw. Deletiong of the binary tree 
  is handled by the distructor. All exceptions 
  thrown are subclasses of expression_exception class
  
  Also provide Calculus class for calculus operations
**********************************************/



#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#if defined MATH
#define MATH_COMMON_DLLSPEC Q_DECL_EXPORT
#else
#define MATH_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

#include <string>
#include <sstream>
#include <map>
#include <functional>
#include <vector>
#include <exception>
#include "Gold/math/utils/utils.hpp"


namespace Gold {
  namespace math {

    //Some exceptions
    class expression_exception: public std::exception {
    public:
      virtual const char* what() { return msg.c_str();}
      virtual std::string error() { return msg;}
      virtual void append(const std::string& str) { msg.append(str);}
    protected:
      std::string msg;
    };

    class var_not_found_exception: public expression_exception { 
    public:
      var_not_found_exception(const std::string& var) {
	msg.append("Error: Variable ");
	msg.append(var);
	msg.append(" not found");
      }
    };

    class wrong_num_vars_exception: public expression_exception {
    public:
      wrong_num_vars_exception(std::size_t given,std::size_t takes) {
	std::ostringstream stream;
	stream << "Error: expression takes " << takes << " entries, " << given << " given";
	msg.append(stream.str());
      } 
    };

    class invalid_expression_exception: public expression_exception {
    public:
      invalid_expression_exception() {
	msg.append("Error: Invalid expression");
      }
    };

    class protected_symbol_exception: public expression_exception {
    public:
      protected_symbol_exception(const std::string& s) {
	msg.append("Error: ").append(s).append(" is a protected symbol");
      }
    };


    
    using utils::Node;
    using utils::KeyString;
    //expression Class
    class expression {
    public:
      expression(std::string expression = "0", const std::vector<std::string>& vars = {}); //Regular/default constructor, throws if expression is
      //Invalid
      expression(const expression& other);        //Copy Constructor
      expression(const expression* const other);     //Copy constructor w/ ptrs
      expression(expression&& other);       //Move constructor
      expression& operator=(const expression& other);  //Copy assignment
      expression& operator=(expression&& other);    //Move assignment
      virtual ~expression();           //Destructor

      virtual double evaluate(const std::vector<double>& x) const;  //Evaluate method to evaluate the tree, will throw if a variable is not found
      //Or if the wrong number of parameters are given
      virtual double operator()() const { return evaluate({}); }  //Throws if expression needs a variable
      virtual double operator()(double x) const;     //Throws if expression needs more than one or nor variables
      virtual double operator()(const std::vector<double>& x) const { return evaluate(x); } 
      virtual std::vector<std::pair<double, double> > operator()(double start, double end, double step = 0.01) const;
      virtual bool is_valid() const { return valid;}          //Returns true if string given in constructor was valid
      virtual std::string get_expression() const { return expr;}   //Returns the string passed into the constructor
      virtual std::string build_expression() const { return remove_enclosing_parens(_build_expression(root)); }   //Builds a new string from the tree
      virtual std::vector<std::string> get_variables() const { return variable;}  //returns a copy of the variables  
      virtual std::size_t get_num_variables() const { return variable.size();}
      virtual std::string change_variable(const std::string& from, const std::string& to) const; //Returns a string with a change of variables
      virtual std::string change_variable(const std::size_t& from, const std::string& to) const; //chages variable number "from" to "to"
      virtual std::string change_variables(const std::vector<std::string>& newVars) const;

      static void add_function(const std::string& str,const expression& expr);
      static bool is_function_in_table(const std::string& str) { return expression_table.find(str) != expression_table.end();}
      static void add_constant(const std::string& str, double x);  //Adds a constant to the symbols table
      static bool is_constant(const std::string& str) { return symbols_table.find(str) != symbols_table.end(); }  //True if a string is a known constant
      static double get_constant(const std::string& str) { return is_constant(str) ? symbols_table[str] : 0;}  //Gets a constant, 0 if not found
      static bool is_valid(std::string e) { return is_string_valid(remove_spaces(e)); }     //True if the string can be used as an expression

      static void swap(expression& a, expression& b);
      virtual void swap(expression& other);

      friend class calculus;
  
      virtual void print_in_order() const; //For Debugging
    private:
      static std::string replace_all(std::string str, const std::string& from, const std::string& to);
      static std::string add_implicit_zeroes(std::string str);
      static std::string remove_spaces(std::string input);
      static std::string remove_enclosing_parens(std::string str);
      static std::string replace_functions(std::string);
      static bool are_parens_matched(const std::string& str);
      static bool is_function(const std::string& str);
      static bool is_string_valid(const std::string& str);
      static int search(const std::string& str, const std::size_t& parenSearchLevel, char key1, char key2);
      static int find_lowest_priority(const std::string& str);
      static bool needs_parens(const std::string& str,const std::string& op,bool RHS);
      static void append_with_parens(std::string& all, const std::string& str, const std::string& op,bool RHS);  

      virtual void load_tree(std::string, int priorityOffset);
      virtual void insert(KeyString item);
      virtual void _insert(KeyString item, Node* node);
      static void destroy(Node* node);
      virtual void _print_in_order(Node* node) const;
      virtual double _evaluate(const std::vector<double>& x, Node* node) const;
      static std::string _build_expression(Node* node);
      virtual void _change_variable(Node* node,const std::string& from, const std::string& to) const;

      std::string expr;
      std::vector<std::string> variable;
      bool valid;
      Node* root;
      static std::map<std::string, double> symbols_table;
      static std::map<std::string, std::function<double(double)> > function_table;
      static std::map<std::string,expression> expression_table;
  
    };

    expression operator+(const expression& a, const expression& b);
    expression operator-(const expression& a, const expression& b);
    expression operator*(const expression& a, const expression& b);
    expression operator/(const expression& a, const expression& b);
   
  }
}
#endif

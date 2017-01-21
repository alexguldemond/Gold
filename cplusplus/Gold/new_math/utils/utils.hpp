#ifndef GOLD_MATH_UTILS
#define GOLD_MATH_UTILS

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Gold {
    namespace math {
	namespace utils {
	    
	    struct node {
		std::string token;
		std::vector<std::shared_ptr<node> > children;
	    };
      
	    typedef std::shared_ptr<node> node_ptr;
	    typedef std::vector<node_ptr> node_vec;
      
	    /**********************************//**
	    * Determines if a given character is
	    * a number.
	    **************************************/
	    bool is_char_num(char ch);

	    /**********************************//**
     	    * Determines if a given string is
	    * a number.
	    **************************************/
	    bool is_string_num(const std::string& str);

	    /**********************************//**
	    * Determines if a string has the given operator
	    * outside of any parentheses.
	    *
	    * str => The string to check.\n
	    * op  => The operator to check for.
	    **************************************/
	    bool has_uncontained_op(const std::string& str, char op);
	    
	    /**********************************//**
	    * Determines if a given string has valid
	    * parentheses
	    **************************************/      
	    bool are_parens_matched(const std::string& str);
      
	    /**********************************//**
	    * Determines if a given string is
	    * a call to a function.
	    * 
	    * Examples:
	    *  Sin[x^2] will return true.
	    *  S[x^2]+1 will return false.
	    **************************************/
	    bool is_function(const std::string& str);

	    /**********************************//**
	    * Determines if a given string needs parentheses
	    * to be properly joined with a larger expression
	    * via the given operator
	    *
	    * str => The string being attached.\n
	    * op => The operator the string is being attached with.\n
	    * RHS => Whether or not str is to be the right hand operand.\n
	    **************************************/
	    bool needs_parens(const std::string& str,const std::string& op,bool RHS);

	    /**********************************//**
	    * Determines if a given vector of strings
	    * contains the given string.
	    *
	    * str => The string to search for.\n
	    * vec => The vector to search through.\n
	    *
	    * Return value: The index of str in vec. 
	    *               -1 if not found.
	    **************************************/      
	    int contains(const std::string& str, const std::vector<std::string>& vec);

	    /*****************************************************************************************//**
	    * Searches for one of the two given keys in a string
	    * at a given depth of parentheses. 0 is not wrapped in parentheses
	    *
	    * str              => The string to search through.\n
	    * parenSearchLevel => The depth to search at.\n
	    * key1, key2       => The two keys to search for.\n
	    *                     Two keys are used since multiple operators have the same priority
	    *
	    * Return value: The index of the found key. -1 if not found
	    *********************************************************************************************/
	    int search(const std::string& str, const std::size_t& parenSearchLevel, char key1, char key2);
	    
	    /*****************************************************************************************//**
	    * Find the index of the lowest priority operator in a given expression.
	    *
	    * str              => The string to search through.
	    *
	    * Return value: The index of the lowest priority operator. -1 if no operator is found, i.e.
	    *               str is just a variable or a number. -2 if the given expression is just a 
	    *               function call.
	    *********************************************************************************************/      
	    int find_lowest_priority(const std::string& str);

	    /*****************************************************************************************//**
	    * Append an expression to another expression using the given operator. 
	    *
	    * all => The larger string that is being appended to. This is a reference and holds the resulting string.\n
	    * str => The string that is appended to all.\n
	    * op  => The operator being used to join all and str.\n
	    * RHS => Whether or not str is the right hand side\n
	    *********************************************************************************************/
	    void append_with_parens(std::string& all, const std::string& str, const std::string& op,bool RHS);
	    
	    /*****************************************************************************************//**
	    * Replace all instances of a substring with a new string.
	    *
	    * str  => The string to do the replacement on.\n
	    * from => The substring to be replaced.\n
	    * to   => The substring to replace with.\n
	    *
	    * Return value: The result of the operation.
	    *********************************************************************************************/
	    std::string replace_all(std::string str, const std::string& from, const std::string& to);
	    
	    /*****************************************************************************************//**
	    * Insert all implicit zeroes from unary plus and minus operators.
	    *
	    * str  => The string to do the replacement on.
	    *
	    * Return value: The result of the operation.
	    *********************************************************************************************/
	    std::string add_implicit_zeroes(std::string str);
	    
	    /*****************************************************************************************//**
	    * Remove all whitespace from a string.
	    *
	    * str  => The string to remove whitespace.
	    *
	    * Return value: The result of the operation.
	    *********************************************************************************************/
	    std::string remove_spaces(std::string str);
	    
	    /*****************************************************************************************//**
	    * Remove parentheses enclosing the entire string. If there are no surrounding parentheses,
	    * this does nothing.
	    *
	    * str  => The string to remove parentheses.
	    *
	    * Return value: The result of the operation.
	    *********************************************************************************************/      
	    std::string remove_enclosing_parens(std::string str);
	    
	    /*****************************************************************************************//**
	    * Simple join function
	    *
	    * joiner => The string to use to jon the elements of the given vector.\n
	    * vector => The vector of string to join together. 
	    *
	    * Return value: The result of the operation.
	    *********************************************************************************************/
	    std::string join_vector(const std::string& joiner, const std::vector<std::string>& vector); 

	    /*****************************************************************************************//**
	    * Split the given string into a vector of strings using the given delimitter.
	    *
	    * str  => The string to split.
	    *
	    * Return value: A vector of strings
	    *********************************************************************************************/           
	    std::vector<std::string> split_string(std::string str, const std::string& delim);
	    
	    /*****************************************************************************************//**
	    * Mathematically break the string using the given operation as the delimter. a+b+3*c turns into
	    * a, b, 3*x when using addition. This only works when the operation passed in is the lowest priority
	    * operation, which can be found with find_lowest_priority
	    *
	    * str       => The string to split.
	    * operation => The operation to break the string with.
	    * vector    => Where to store the results.
	    *
	    * Return value: A vector of strings
	    *********************************************************************************************/           
	    void break_string(std::string str, const std::string& , std::vector<std::string>& vector);

	    /*****************************************************************************************//**
	    * Determines if the given node is a leaf.
	    *
	    * node  => The node to test.
	    *
	    * Return value: A vector of strings
	    *********************************************************************************************/        
	    bool is_leaf(node_ptr node);
	    
	    /*****************************************************************************************//**
	    * Loads the given expression into a tree with root being the head of the tree.
	    *
	    * string => The string to push into the tree.\n
	    * root   => The root of the tree.\n 
	    *
	    * Return value: A vector of strings
	    *********************************************************************************************/
	    void load_tree(std::string string, node_ptr root);
	    
	    /*****************************************************************************************//**
	    * Change the variables in the tree.
	    *
	    * root  => The root of the tree to change.\n
	    * rules => The change in variables. Keys are existing variables, values are what to change to.\n 
	    *
	    * Return value: A vector of strings
	    *********************************************************************************************/
	    void change_variables(node_ptr root, const std::map<std::string, std::string>& rules);

	    /*****************************************************************************************//**
	    * Evaluates the given tree using the given parameters in the map.
	    *
	    * root   => The root of the tree.\n 
	    * params => A map of the params used in the tree.\n
	    * Return value: A vector of strings
	    *********************************************************************************************/              
	    double evaluate(const node_ptr& root, const std::map<std::string, double>& params);
	    
	    /*****************************************************************************************//**
	    * Turns the tree back into a string to be used else where.
	    *
	    * root   => The root of the tree.\n
	    *********************************************************************************************/
	    std::string build_string_from_tree(const node_ptr& root);
	    
	    /*****************************************************************************************//**
	    * A mapping of symbols to values
	    *********************************************************************************************/              
	    extern std::map<std::string, double> symbols_table;
	    
	    /*****************************************************************************************//**
	    * A mapping of symbols to functions
	    *********************************************************************************************/
	    extern std::map<std::string, std::function<double(double)> > function_table;
      
	}
    }
}

#endif

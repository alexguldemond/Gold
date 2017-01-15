#ifndef GOLD_MATH_UTILS
#define GOLD_MATH_UTILS

#include <memory>
#include <string>
#include <vector>

namespace Gold {
  namespace math {
    namespace utils {
      
      struct node;
      typedef std::shared_ptr<node> node_ptr;
      typedef std::vector<node> node_vec;
      
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
      * str => The string to check.
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
      * str => The string being attached.
      * op => The operator the string is being attached with
      * RHS => Whether or not str is to be the right hand operand
      **************************************/
      bool needs_parens(const std::string& str,const std::string& op,bool RHS);

      /**********************************//**
      * Determines if a given vector of strings
      * contains the given string.
      *
      * str => The string to search for.
      * vec => The vector to search through.
      *
      * Return value: The index of str in vec. 
      *               -1 if not found.
      **************************************/      
      int contains(const std::string& str, const std::vector<std::string>& vec);

      /*****************************************************************************************//**
      * Searches for one of the two given keys in a string
      * at a given depth of parentheses. 0 is not wrapped in parentheses
      *
      * str              => The string to search through
      * parenSearchLevel => The depth to search at.
      * key1, key2       => The two keys to search for.
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
      * Find the index of the lowest priority operator in a given expression.
      *
      * str              => The string to search through.
      *
      * Return value: The index of the lowest priority operator. -1 if no operator is found, i.e.
      *               str is just a variable or a number. -2 if the given expression is just a 
      *               function call.
      *********************************************************************************************/      
      void append_with_parens(std::string& all, const std::string& str, const std::string& op,bool RHS);

      std::string replace_all(std::string str, const std::string& from, const std::string& to);
      
      std::string add_implicit_zeroes(std::string str);
      
      std::string remove_spaces(std::string input);
      
      std::string remove_enclosing_parens(std::string str);
     
      std::vector<std::string> split_string(std::string str, const std::string& delim);
      
      bool is_leaf(node_ptr node);

    }
  }
}

#endif

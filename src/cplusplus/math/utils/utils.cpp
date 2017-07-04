#include "Gold/math/utils.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <regex>
#include <sstream>
#include <stack>
#include <stdexcept>

namespace Gold {
    namespace math {
	namespace utils {
      
	    int quotient(int a, int b) {
		return a / b;
	    }

	    int remainder(int a, int b) {
		return a % b;
	    }

	    bool divides(int a, int b) {
		return remainder(b,a) == 0;
	    }

	    int greatest_common_divisor(int a, int b) {
		int r = 0;
		while ( b != 0 ) {
		    r = remainder(a,b);
		    a = b;
		    b = r;
		}
		return abs(a);
	    }

	    bool are_relatively_prime(int a, int b) {
		return greatest_common_divisor(a,b) == 1;
	    }

	    int least_common_multiple(int a, int b) {
		return (a * b)/greatest_common_divisor(a,b);
	    }

	    std::pair<int, int> simplify_rational_number(int num, int den) {
		int gcd = greatest_common_divisor(num, den);
		if (den > 0) {
		    return { num / gcd, den / gcd };
		} else {
		    return { -num / gcd, -den / gcd };
		}
	    }

	    bool is_char_num(char ch) {
		return (ch >= '0' && ch <= '9');
	    }

	    bool is_string_integer(const std::string& s) {
		if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

		char * p ;
		strtol(s.c_str(), &p, 10) ;                                                         

		return (*p == 0) ;
	    }


	    bool is_string_num(const std::string& str) {
		if (str == "") {
		    return false;
		} else if (str.size() == 1 && !is_char_num(str[0])) {
		    return false;
		} else if (str[0] != '-' && str[0] != '.' && !is_char_num(str[0])) {
		    return false;
		}
		bool oneDecimal = false;
		for (auto it = str.begin()+1; it != str.end(); it++) {
		    if (is_char_num(*it)) continue;
		    if (*it == '.' && !oneDecimal) {
			oneDecimal = true;
			continue;
		    }
		    return false;
		}
		return true;
	    }
      
	    bool is_string_var(const std::string& var) {
		return std::regex_match(var, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"));
	    }

	    bool has_uncontained_op(const std::string& str, char op) {
		int parenLevel = 0;
		for (auto it = str.begin(); it != str.end(); it++) {
		    if (*it == '(' || *it == '[') {
			parenLevel--;
		    } else if (*it == ')' || *it == ']') {
			parenLevel++;
		    } else if (parenLevel >= 0 && *it == op) {
			return true;
		    }
		}
		return false;
	    }

	    bool are_parens_matched(const std::string& str) {
		std::stack<char> stack;
		for (auto iter = str.begin(); iter != str.end(); iter++) {
		    if (*iter == '(') {
			stack.push('(');
		    } else if (*iter == '[') {
			stack.push('[');
		    } else if (*iter == ')') {
			if (stack.empty()) {
			    return false;
			} else if (stack.top() == '[') {
			    return false;
			} else {
			    stack.pop();
			}
		    } else if (*iter == ']') {
			if (stack.empty()) {
			    return false;
			} else if (stack.top() == '(') {
			    return false;
			} else {
			    stack.pop();
			}
		    }
		}
		if (!stack.empty()) {
		    return false;
		} else {
		    return true;
		}
	    }
      
	    bool is_function(const std::string& str) {
		if (str.size() < 4) {
		    return false;
		} else if (str[str.size()-1] != ']') {
		    return false;
		} else if (!are_parens_matched(str)) {
		    return false;
		} else {
		    std::string sub(str.substr(0,str.find("[")));
		    if (sub.size() == 0) {
			return false;
		    } else if (!std::isalnum(sub[0])) {
			return false;
		    } else if (sub.find("+") != std::string::npos ||
			       sub.find("-") != std::string::npos ||
			       sub.find("*") != std::string::npos ||
			       sub.find("/") != std::string::npos ||
			       sub.find("^") != std::string::npos) {
			return false;
		    } else {
			int bracketLevel = 0;
			for (std::size_t i = 0; i < str.size(); i++) {
			    if (str[i] == '[') {
				bracketLevel++;
			    } else if (str[i] == ']') {
				bracketLevel--;
				if (bracketLevel == 0 && i != str.size() -1) {
				    return false;
				}
			    }
			}
		    }
		    return true;
		}
	    }
    
	    bool needs_parens(const std::string& str,const std::string& op,bool RHS) {
		if (is_function(str) || op =="+" || (op == "-" && !RHS)) {
		    return false;
		} 
		else if ( (op == "-" && RHS) || op == "*" || (op == "/" && !RHS)) {
		    return has_uncontained_op(str,'+') || has_uncontained_op(str,'-');
		} 
		else if ( (op == "/" && RHS) || (op == "^" && RHS) ) {
		    return (has_uncontained_op(str,'+') || has_uncontained_op(str,'-') ||
			    has_uncontained_op(str,'*') || has_uncontained_op(str,'/'));
		} 
		else if (op == "^" && !RHS) {
		    return (has_uncontained_op(str,'+') || has_uncontained_op(str,'-') ||
			   has_uncontained_op(str,'*') || has_uncontained_op(str,'/') ||
			   has_uncontained_op(str, '^'));
		} 
		return true;
	    }

	    int search(const std::string& str, const std::size_t& parenSearchLevel, char key1, char key2) {
		int index = -1;
		std::size_t parenLevel = 0;
		std::size_t bracketLevel = 0;
		
		//Exponentiation is evaluated right to left
		if (key1 == '^' || key2 == '^') {
		    int count = str.size() - 1;
		    for (auto iter = str.rbegin(); iter != str.rend(); iter++) {
			if (*iter == ']') {
			    bracketLevel++;
			} else if (*iter == '[') {
			    bracketLevel--;
			} else if (*iter == ')') {
			    parenLevel++;
			} else if (*iter == '(') {
			    parenLevel--;
			}
			if (parenLevel == parenSearchLevel && bracketLevel == 0) {
			    if (*iter == key1 || *iter == key2) {
				index = count;
			    }
			}
			count--;
		    }
		    return index;
		}
		
		//Everything else is left to right
		else {
		    int count = 0;
		    for (auto iter = str.begin(); iter != str.end(); iter++) {
			if (*iter == '[') {
			    bracketLevel++;
			} else if (*iter == ']') {
			    bracketLevel--;
			} else if (*iter == '(') {
			    parenLevel++;
			} else if (*iter == ')') {
			    parenLevel--;
			}
			if (parenLevel == parenSearchLevel && bracketLevel == 0) {
			    if (*iter == key1 || *iter == key2) {
				index = count;
			    }
		    }
			count++;
		    }
		    return index;
		}
	    }

	    bool is_binary(int index, const std::string& str) {
		if (index < 0 || index >= (int)str.size()) {
		    throw std::length_error("Index out of bounds");
		}
		if (index == 0) {
		    return false;
		}
		char lhs = str[index-1];;
		return (lhs != '+' && lhs != '-' && lhs != '*' && lhs != '/' &&
			lhs != '^' && lhs != ',' && lhs != '(' && lhs != '[');
	    }

	    int find_lowest_priority(const std::string& str) {
		if (is_function(str)) {
		    return -2;
		}
		std::size_t parenSearchLevel = 0;
		std::size_t maxParenLevel = std::count(str.begin(), str.end(), '(');
		int index = -1;
		while (index == -1 && maxParenLevel >= parenSearchLevel) {
		    index = search(str,parenSearchLevel,'+','-');
		    if ( index >= 0 && !is_binary(index, str)) index = -1; //Error handling of negative
		    if (index != -1) break;
		    index = search(str,parenSearchLevel,'*','/');
		    if (index != -1) break;
		    index = search(str,parenSearchLevel,'^','#');
		    parenSearchLevel++;
		}
		return index;
	    }

	    void append_with_parens(std::string& all,const std::string& str,const std::string& op,bool RHS) {
		if (needs_parens(str,op,RHS)) {
		    all.append(op).append("(").append(str).append(")");
		} else {
		    all.append(op).append(str);
		}
	    }

	    std::string remove_enclosing_parens(std::string str) {
		if (str.size() <= 1) {
		    return str;
		}
		if (str[0] != '(' || str[str.size() - 1] != ')') {
		    return str;
		}
		int parenLevel = -1;
		std::size_t closeIndex = 0;
		for (std::size_t i = 1; i < str.size(); i++) {
		    if ( str[i] == '(') {
			parenLevel--;
		    } else if (str[i] == ')') {
			parenLevel++;
			if (parenLevel == 0) {
			    closeIndex = i;
			    break;
			}
		    }
		}
		if (closeIndex == str.size() -1) {
		    return remove_enclosing_parens(str.substr(1,str.size() - 2));
		} else {
		    return str;
		}
	    }
   
	    void break_string(std::string str, const std::string& operation, std::vector<std::string>& vector, bool invert) {
		if (operation != "+" && operation != "-" &&
		    operation != "*" && operation != "/") {
		    throw std::invalid_argument(operation + " can't be used to break the string");
		}
		str = remove_enclosing_parens(str);
		int index = find_lowest_priority(str);
		std::string field_operation; 
		std::string inverse_operation;
		if (operation == "+" || operation == "-") {
		    field_operation = "+";
		    inverse_operation = "-";
		}
		else /*if (operation == "*" || operation == "/")*/ {
		    field_operation = "*";
		    inverse_operation = "/";
		}
		std::string found_operation = std::string(1,str[index]);
		if ( found_operation == field_operation || found_operation == inverse_operation) {
		    std::string lhs = str.substr(0, index);
		    std::string rhs = str.substr(index+1, str.size()-1);
		    break_string(lhs, operation, vector, invert);
		    break_string(rhs, operation, vector, invert != (found_operation == inverse_operation) );
		} 
		else {
		    // Negative values
		    bool isPositive = true;
		    if (str[0] == '-') {
			std::string opposite = str.substr(1, str.size()-1);
			if (!is_string_num(opposite)) {
			    str = opposite;
			    isPositive = false;
			}
		    }
		    
		    if ( operation == "+" || operation == "-") {
			if ( invert == isPositive ) {
			    str.insert(0, "(-1)*(").append(")");
			}
		    }
		    else /*if (operation == "*" || operation == "/")*/ {
			if (invert) {
			    str.insert(0, "(").append(")^(-1)");
			}
			if ( !isPositive ) {
			    str = std::string("(-1)*").append(str);
			}
		    }
		    vector.push_back(str);
		    return;
		} 
	    }
      
	}
    }
}

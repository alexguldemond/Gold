#include "Gold/new_math/utils/utils.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream> //Remove later, replace error messagas with exceptions
#include <sstream>
#include <stack>

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

	    int contains(const std::string& str, const std::vector<std::string>& vec) {
		int index = -1;
		int count = 0;
		for (auto it = vec.begin(); it != vec.end(); it++) {
		    if (*it == str) {
			index = count;
			break;
		    }
		    count++;
		}
		return index;
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
	    int find_lowest_priority(const std::string& str) {
		if (is_function(str)) {
		    return -2;
		}
		std::size_t parenSearchLevel = 0;
		std::size_t maxParenLevel = std::count(str.begin(), str.end(), '(');
		int index = -1;
		while (index == -1 && maxParenLevel >= parenSearchLevel) {
		    index = search(str,parenSearchLevel,'+','-');
		    if (index == 0 && str[0] == '-') index = -1; //Error correction
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

	    std::string replace_all(std::string str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		    str.replace(start_pos, from.length(), to);
		    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'                     
		}
		return str;
	    }

	    std::string add_implicit_zeroes(std::string str) {
		if (str == "") {
		    return "0";
		}
		str = replace_all(replace_all(str,"(+","(0+"),"(-","(0-");
		if (str[0] == '-' || str[0] == '+') {
		    std::string z = "0";
		    str = z.append(str);
		}
		return str;
	    }

	    std::string remove_spaces(std::string input) {
		input.erase(std::remove(input.begin(),input.end(),' '),input.end());
		return input;
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
      
	    std::string join_vector(const std::string& joiner, const std::vector<std::string>& vector) {
		if (vector.empty()) {
		    return "";
		}
		std::string result = vector[0];
		for (auto it = vector.begin()+1; it != vector.end(); it++) {
		    result.append(joiner).append(*it);
		}
		return result;
	    }
		    
	    std::vector<std::string> split_string(std::string str, const std::string& delim) {
		std::vector<std::string> args;
		while ((str.find(delim)) != std::string::npos) {
		    int i = str.find(delim);
		    args.push_back(str.substr(0,i));
		    str = str.substr(i+1,str.size()-i);
		}
		args.push_back(str);
		return args;
	    }
   
	    void break_string(std::string str, const std::string& operation, std::vector<std::string>& vector, bool invert) {
		if (operation != "+" && operation != "-" &&
		    operation != "*" && operation != "/") {
		    throw operation + " can't be used to break the string";
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
		    if (invert) {
			if (operation == "+" || operation == "-") {
			    str.insert(0, "(-1)*(").append(")");
			}
			else /*if (operation == "*" || operation == "/")*/ {
			    str.insert(0, "(").append(")^(-1)");
			}
		    }
		    vector.push_back(str);
		    return;
		} 
	    }

	    std::map<std::string, double> symbols_table = {
		{"PI",atan(1)*4},
		{"e",2.71828182845904523536}};

	    std::map<std::string, std::function<double(double)> > function_table = {
		{"Sin", sin},
		{"Cos",cos},
		{"Tan",tan},
		{"Csc", [](double x) -> double {return 1.0/sin(x);}},
		{"Sec", [](double x) -> double {return 1.0/cos(x);}},
		{"Cot", [](double x) -> double {return 1.0/tan(x);}},
		{"ArcSin", asin},
		{"ArcCos", acos},
		{"ArcTan", atan},

		{"Sinh",sinh},
		{"Cosh", cosh},
		{"Tanh", tanh},
		{"ArcSinh", asinh},
		{"ArcCosh",  acosh},
		{"ArcTanh", atanh},

		{"ln", log},
		{"Log", log10},
		{"Log2", log2},
		{"Sqrt",sqrt},
		{"Abs",[](double x) -> double {return (x >= 0) ? x : -x;}},
		{"H",[](double x) -> double {return (x >= 0) ? 1 : 0;}}
	    };
      
	}
    }
}

#include "utils.hpp"
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
		return { num / gcd, den / gcd };
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
   
	    void break_string( std::string str, const std::string& operation, std::vector<std::string>& vector, bool invert) {
		if (operation != "+" && operation != "-" &&
		    operation != "*" && operation != "/") {
		    throw operation + " can't be used to break the string";
		}
		std::string real_operation = operation;
		if (operation == "/") {
			real_operation = "*";
		}
		if (operation == "-") {
		    real_operation = "+";
		}
		std::string inverse_operation = real_operation == "+" ? "-" : "/";
		std::string safe_beginning = real_operation == "+" ? "0" : "1";
		str = remove_enclosing_parens(str);
		int index = find_lowest_priority(str);
		bool str_is_inverted = (str.substr(0,2) == safe_beginning + inverse_operation && index == 1);
	        
#ifdef DEBUG
		std::cout << "Str = " << str << "\n";
		std::cout << "Operation = " << operation << "\n";
		std::cout << "Invert = " << invert << "\n";
		std::cout << "real operation = " << real_operation << "\n";
		std::cout << "safe beginning = " << safe_beginning << "\n";
		std::cout << "Is string inverted = " << str_is_inverted << "\n";
#endif
		
		if (index < 0) {
		    if (invert) {
			std::string temp = safe_beginning;
			append_with_parens(temp, str, inverse_operation, true);
			str = temp;
		    }
#ifdef DEBUG
		    std::cout << "Pushing " << str << "\n"; 
		    std::cout << "\n\n";
#endif
		    vector.push_back(str);
		    return;
		}
		
		if (str_is_inverted) {
		    if (invert) {
			std::string temp = safe_beginning;
			str = remove_enclosing_parens( str.substr(2, str.size()-2) );
			str = temp;
		    }
#ifdef DEBUG
		    std::cout << "Pushing " << str << "\n"; 
		    std::cout << "\n\n";
#endif
		    vector.push_back(str);
		    return;
		}

		std::string new_operation;
		std::stringstream stream;
		stream << str[index];
		stream >> new_operation;
		
		if ( new_operation != operation && new_operation != inverse_operation) {
		    if (invert) {
		        std::string temp = safe_beginning;
			append_with_parens(temp, str, inverse_operation, true);
			str = temp;
		    }
#ifdef DEBUG
		    std::cout << "Pushing " << str << "\n"; 
		    std::cout << "\n\n";
#endif
		    vector.push_back(str);
		    return;
		}
		
		std::string substr1(remove_enclosing_parens(str.substr(0,index)));
		
		std::string substr2(remove_enclosing_parens(str.substr(index+1,str.size()-index)));
		
		break_string(substr1, operation, vector, invert);
		break_string(substr2, operation, vector, (operation == inverse_operation) ? !invert : invert);
	    }

	    bool is_leaf(node_ptr node) {
		if (!node) {
		    return false;
		}
		if (node->children.empty()) {
		    return true;
		}
		return false;
	    }

	    double node_hash(node_ptr node) {
		if (!node) {
		    return -2.0;
		}
		std::string token = node->token;
		if (is_string_num(token)) {
		    double num = atof(token.c_str());
		    return (num > 0) ? num / (1 + num) : num / (1 - num);
		}
		if (is_leaf(node)) {
		    double num = 1.0;
		    for (auto iter = token.begin(); iter != token.end(); iter++) {
			num += *iter;
		    }
		    return (num > 0) ? num / (1 + num) : num / (1 - num);
		}
		if (token != "+" && token != "-" && token != "*" && token != "/" && token != "^") {
		    return 2.0;
		}
		double num = 3.0;
		std::vector<std::string> ops = { "+", "-", "*", "/", "^" };
		for (unsigned int i = 0; i < ops.size(); i++) {
		    if (token == ops[i]) {
			return num + i;
		    }
		}
		return 8.0;
	    }
	    
	    int kind(node_ptr root) {
		if (!root) {
		    return UNDEFINED;
		} else if (is_leaf(root)) {
		    if      ( is_string_integer(root->token)) { return INTEGER; }
		    else if ( is_string_num(root->token))     { return NUMBER;  }
		    else                                      { return SYMBOL;  }
		}
		const std::string& token = root->token;
		const std::vector<node_ptr>& children = root->children;
		if (token == "-") {
		    return SUBTRACT;
		} else if (token == "^") {
		    return POWER;
		} else if (token == "+") {
		    if (children.size() != 2) {
			return ADD;
		    } else {
			node_ptr right = children[1];
			return (right->token == "*" && 
				right->children.size() == 2 && 
				right->children[0]->token == "-1") ? SUBTRACT : ADD;
		    }
		} else if (token == "*") {
		    if (children.size() != 2) {
			return MULTIPLY;
		    } else {
			node_ptr right = children[1];
			if (right->token == "^" && 
			    right->children.size() == 2 && 
			    right->children[1]->token == "-1") {
			    node_ptr numerator = children[0];
			    node_ptr denominator = right->children[0];
			    return ( is_leaf(numerator) && 
				     is_leaf(denominator) &&
				     is_string_integer(numerator->token) &&
				     is_string_integer(denominator->token)) ? FRACTION : DIVIDE;
			} else {
			    return MULTIPLY;
			}
		    }	
		} else if (token == "/") {
		    node_ptr numerator = children[0];
		    node_ptr denominator = children[1];
		    return ( is_leaf(numerator) && 
			     is_leaf(denominator) &&
			     is_string_integer(numerator->token) &&
			     is_string_integer(denominator->token)) ? FRACTION : DIVIDE;
		} else {
		    return FUNCTION;
		}
	    }

	    bool node_compare(node_ptr left, node_ptr right) {
		return node_hash(left) < node_hash(right);
	    }
		
	    void load_tree(std::string string, node_ptr root) {
		string = remove_spaces(string);
		string = add_implicit_zeroes(string);
		string = remove_enclosing_parens(string);
		int index = find_lowest_priority(string);

		//If the string is just a number
		if (index == -1) {
		    root->token = string;
		    return;
		}

		//If the string is a function
		else if (index == -2) {
		    std::string newString( string.substr(string.find("[")+1, string.size()-2-string.find("[")) );
		    std::string funcName( string.substr(0,string.find("[")));
		    root->token = funcName;

		    std::vector<std::string> args = split_string(newString, ",");
		    for (auto it = args.begin(); it != args.end(); it++) {
			node_ptr child = std::make_shared<node>();
			root->children.push_back(child);
			load_tree(*it, child);
		    }
		}
	
		//If this is a simple expression
		else {
		    std::string operation;
		    std::stringstream stream;
		    stream << string[index];
		    stream >> operation;
		    
		    std::string real_operation;
		    if (operation == "/") real_operation = "*"; 
		    else if (operation == "-") real_operation = "+";
		    else real_operation = operation;
		    
		    std::string safe_beginning = real_operation == "+" ? "0" : "1";
  
		    bool str_is_inverted = (string.substr(0,2) == safe_beginning + operation 
					    && index == 1
					    && operation != "^"
					    && operation != real_operation);
		    if (str_is_inverted || operation == "^") {
			std::string substr1(string.substr(0,index));
			std::string substr2(string.substr(index+1,string.size()-index));
			
			root->token = operation;
			node_ptr left_child = std::make_shared<node>();
			node_ptr right_child = std::make_shared<node>();
			
			root->children.push_back(left_child);
			root->children.push_back(right_child);
			
			load_tree(substr1, left_child);
			load_tree(substr2, right_child);
		    }

		    //Recursive search only for associative and commutative operations
		    else {
			std::vector<std::string> vec;
			root->token = real_operation;
		      
			break_string(string, operation, vec, false);
			
			root->children.reserve(vec.size());
			for (auto it = vec.begin(); it != vec.end(); it++) {
			    node_ptr child = std::make_shared<node>();
			    root->children.push_back(child);
			    load_tree(*it, child);
			}
		    }
		}
		std::sort(root->children.begin(), root->children.end(), node_compare);
	    }

	    void change_inverse_method(node_ptr root) {
		if ( !root || is_leaf(root)) return;
		if (root->token == "-" && root->children[0]->token == "0") {
		    root->token = "*";
		    root->children[0]->token = "-1";
		} else if (root->token == "/" && root->children[0]->token == "1") {
		    root->token = "^";
		    std::swap(root->children[0], root->children[1]);
		    root->children[1]->token = "-1";
		}
		for (auto iter = root->children.begin(); iter != root->children.end(); iter++) {
		    change_inverse_method(*iter);
		}
	    }

	    void simplify_tree(node_ptr root) {
		if ( !root ) {
		    return;
		}
		if ( is_leaf(root) ) {
		    return;
		}
		
		for (auto iter = root->children.begin(); iter != root->children.end(); iter++) {
		    simplify_tree(*iter);
		}
		
		if      (root->token == "^") { return simplify_power(root); }
		else if (root->token == "*") { return simplify_product(root); }
		//else if (root->token == "+") { return simplify_sum(root); }
	    }

	    void simplify_power(node_ptr root) {
		if ( !root || is_leaf(root) || root->token != "^") {
		    return;
		}
		node_ptr base = root->children[0];
		node_ptr expo = root->children[1];
		std::string new_token;
		if ( is_string_num(base->token) && is_string_num(expo->token) ) {
		    //0^0 will error
		    new_token = std::pow(atof(base->token.c_str()), atof(expo->token.c_str()));
		    root->token = new_token;
		    root->children.clear();
		    return;
		}
		if (base->token == "0") {
		    new_token = "0";
		    root->token = new_token;
		    root->children.clear();
		    return;
		}
		if (base->token == "1") {
		    new_token = "1";
		    root->token = new_token;
		    root->children.clear();
		    return;
		}
		if ( is_string_integer(expo->token) ) {
		    simplify_integer_power(root);
		    return;
		}
	    }

	    void simplify_integer_power(node_ptr root) {
		if ( !root || is_leaf(root) || root->token != "^") {
		    return;
		}
		node_ptr base = root->children[0];
		node_ptr expo = root->children[1];
		std::string new_token;
		
		if ( is_leaf(base) ) {
		    return;
		}
		
		if ( expo->token == "0") {
		    new_token = "1";
		    root->token = new_token;
		    root->children.clear();
		    return;
		}
		if ( expo->token == "1") {
		    root = base;
		    return;
		}
		if (base->token == "^") {
		    node_ptr base_base = base->children[0];
		    node_ptr base_expo = base->children[0];
		    node_ptr p = std::make_shared<node>();
		    p->token = "*";
		    p->children = { base_expo, expo };
		    simplify_product(p);
		    root->children = { base_base, p };
		    if ( is_string_integer(p->token) ) {
			simplify_integer_power(root);
		    }
		    return;
		}
		if (base->token == "*") {
		    for (auto iter = base->children.begin(); iter != base->children.end(); iter++) {
			node_ptr old_ptr = *iter;
			node_ptr new_ptr = std::make_shared<node>();
			node_ptr new_expo = std::make_shared<node>();
			
			new_ptr->token = "^";
			new_expo->token = expo->token;
			new_ptr->children = {old_ptr, new_expo};
			
			(*iter) = new_ptr;
		    }
		    root = base;
		    simplify_product(root);
		}
	    }

	    void simplify_product(node_ptr root) {
	    }

		
	    void change_variables(node_ptr root, const std::map<std::string, std::string>& rules) {
		if (!root) {
		    return;
		}
		if (is_leaf(root)) {
		    if ( rules.find( root->token ) != rules.end() ) {
			load_tree( rules.at(root->token), root );
		    }
		}
		else {
		    for (auto it = root->children.begin(); it != root->children.end(); it++) {
			change_variables(*it, rules);
		    }
		}
	    }

	    double evaluate(const node_ptr& root,const std::map<std::string, double>& params) {
		if (!root) {
		    std::cerr << "Root is null pointer, returning zero\n";
		    return 0;
		}
	
		const std::string& token = root->token;

		//Number or variable
		if (is_leaf(root)) {
		    if (params.find(token) != params.end()) {
			return params.at(token);
		    } 
		    else if (symbols_table.find(token) != symbols_table.end()) {
			return symbols_table.at(token);
		    }
		    else if (is_string_num(token)) {
			return atof(token.c_str());
		    }
		    else {
			throw token + " can't be interpretted as a variable or a number\n";
		    }
		}

		//Function
		else if (token != "+" && token != "-" && token != "*" && token != "/" && token != "^") {
		    if (function_table.find(token) != function_table.end()) {
			return function_table.at(token)(evaluate(root->children[0], params));
		    }
		    else {
			throw token + " can;t be interpretted as a function name\n";
			return 0;
		    }
		}
	
		//Operator
		else {
		    std::vector<double> vals;
		    vals.reserve(root->children.size());
	  
		    for ( auto it = root->children.begin(); it != root->children.end(); it++) {
			vals.push_back( evaluate(*it, params) );
		    }
	  
#ifdef DEBUG
		    std::cout << "token: " << token << "\t";
		    for (auto it = vals.begin(); it != vals.end(); it++) {
			std::cout << *it << ", ";
		    }
		    std::cout << " result = ";
#endif	 
 
		    double result;
		    if (token != "^") {
			result = *(vals.begin());
			for (auto it = vals.begin()+1; it != vals.end(); it++) {
			    if      (token == "+") { result += *it; }
			    else if (token == "-") { result -= *it; }
			    else if (token == "*") { result *= *it; }
			    else if (token == "/") { result /= *it; }
			    else {
				throw token + " can't be parsed as an operator\n";
			    }
			}
		    }

		    //Token = ^ requires different logic due to how exponentiation works
		    else {
			result = *(vals.rbegin());
			for (auto it = vals.rbegin()+1; it != vals.rend(); it++) {
			    result = pow(*it, result);
			}
		    }
#ifdef DEBUG
		    std::cout << result << "\n";
#endif
		    return result;
	    
		}
	    }

	    std::string build_string_from_tree(const node_ptr& root) {
		if (!root) {
		    return "";
		}
		else if (is_leaf(root)) {
		    return root->token;
		}
		
		std::vector<std::string> args;
		args.reserve(root->children.size());
		for (auto it = root->children.cbegin(); it != root->children.cend(); it++) {
		    args.push_back(build_string_from_tree(*it));
		}
		
		//If node is a function call
		std::string token = root->token;
		if (token != "+" && token != "-" && token != "*" && token != "/" && token != "^") {
		    std::string string = root->token;
		    string.append("[");
		    string.append(join_vector(", ", args)).append("]");
		    return string;
		}

		for (auto it = args.begin(); it != args.end(); it++) {
		    if (needs_parens(*it, token , it != args.begin())) {
			*it = std::string("(").append(*it).append(")");
		    }
		}
		return join_vector(token, args);
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

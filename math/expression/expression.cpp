#include <iostream>
#include <stack>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <utility>
#include "expression.hpp"

namespace Gold {
  namespace math {
   
    //============================================
    //=== Constructors,Copiers, and Destructor ===
    //============================================
    expression::expression(std::string expression, const std::vector<std::string>& vars) { 
      expression = remove_spaces(expression);
      if (!are_parens_matched(expression)) {
	throw invalid_expression_exception();
      }
      expression = replace_functions(expression);
      valid = is_string_valid(expression);
      if (!valid) {
	throw invalid_expression_exception();
      }
      expression = add_implicit_zeroes(expression);
      expression = remove_enclosing_parens(expression);
      variable = vars;
      expr = expression;
      root = nullptr;
      load_tree(expression,0);
    }

    expression::expression(const expression& other) : expr(other.expr), variable(other.variable) {
      valid = other.valid;
      root = nullptr;
      load_tree(expr,0);
    }

    expression::expression(const expression* const other) : expr(other->expr), variable(other->variable) {
      valid = other->valid;
      root = nullptr;
      load_tree(expr,0);
    }
    
    expression::expression(expression&& other) : expr(std::move(other.expr)), variable(std::move(other.variable)) {
      valid = other.valid;
      root = other.root;
      other.root = nullptr;
    }

    expression& expression::operator=(const expression& other) {
      variable = other.variable;
      expr = other.expr;
      valid = other.valid;
      Node* origPtr = root;
      root = nullptr;
      load_tree(expr,0);
      destroy(origPtr);
      origPtr = nullptr;
      return *this;
    }

    expression& expression::operator=(expression&& other) {
      variable = std::move(other.variable);
      expr = std::move(other.expr);
      valid = other.valid;
      Node* origPtr = root;
      root = other.root;
      other.root = origPtr;
      return *this;
    }

    expression::~expression() {
      destroy(root);
      root = nullptr;
    }

    //==========================
    //=== Operator Overloads ===
    //==========================
    expression operator+(const expression& a, const expression& b) {
      std::string newString = "(";
      newString = newString.append(a.get_expression()).append(")+(").append(b.get_expression()).append(")");
      return expression(newString,a.get_variables());    
    }

    expression operator-(const expression& a, const expression& b) {
      std::string newString = "(";
      newString = newString.append(a.get_expression()).append(")-(").append(b.get_expression()).append(")");
      return expression(newString,a.get_variables());
    }

    expression operator*(const expression& a, const expression& b) {
      std::string newString = "(";
      newString = newString.append(a.get_expression()).append(")*(").append(b.get_expression()).append(")");
      return expression(newString,a.get_variables());
    }

    expression operator/(const expression& a, const expression& b) {
      std::string newString = "(";
      newString = newString.append(a.get_expression()).append(")/(").append(b.get_expression()).append(")");
      return expression(newString,a.get_variables());
    }

    //======================
    //=== Public methods ===
    //======================
    //Evaluates the expression with the given variable,throws if wrong number of variables are given
    double expression::evaluate(const std::vector<double>& x) const {
      if (x.size() != variable.size()) {
	wrong_num_vars_exception e(x.size(),variable.size());
	throw e;
      } else {
	return _evaluate(x,root);
      }
    }

    double expression::operator()(double x) const {
      if (variable.size() > 1) {
	wrong_num_vars_exception e(1,variable.size());
	throw e;
      }
      return evaluate({x});
    }

    std::vector<std::pair<double, double> > expression::operator()(double start, double end, double step) const { 
      if (variable.size() > 1) {
	wrong_num_vars_exception e(1,variable.size());
	throw e;
      }
      std::vector<std::pair<double, double> > points;
      points.reserve( static_cast<int>( (end - start)/step + 1) );
      for (double x = start; x < end; x += step) {
	points.push_back( std::make_pair(x,evaluate({x})) );
      }
      return points;
    }

    //Return a std::string with a change of variables done
    std::string expression::change_variable(const std::string& from, const std::string& to) const {
      if (utils::contains(from,this->variable) == -1 || from == to) {
	return this->build_expression();
      } else {
	if (!is_valid(to)) {
	  invalid_expression_exception e;
	  e.append(". Cannot change variable to ");
	  e.append(to);
	  throw e;
	}
	expression newExpr(*this);
	_change_variable(newExpr.root,from,to);
	return newExpr.build_expression();
      }
    }

    std::string expression::change_variable(const std::size_t& from, const std::string& to) const {
      if (from >= variable.size()) return this->build_expression();
      return change_variable(variable[from],to);
    }
  
    std::string expression::change_variables(const std::vector<std::string>& newVars) const {
      if (this->variable.size() == 0) {
	return this->build_expression();
      }
      expression newExpr(*this);
      std::size_t count = 0;
      for (auto iter = newVars.begin(); iter != newVars.end(); iter++) {
	if (count >= newExpr.variable.size()) {
	  break;
	}
	if (newExpr.variable[count] == *iter) {
	  count++;
	} else if (!is_valid(*iter)) {
	  invalid_expression_exception e;
	  e.append(". Cannot change variable to ");
	  e.append(*iter);
	  throw e;
	} else {
	  _change_variable(newExpr.root,newExpr.variable[count],*iter);
	  count++;
	}
      }
      return newExpr.build_expression();
    }


    //======================
    //=== String methods ===
    //======================
    //Replace all occurences of a substring with a different substring
    std::string expression::replace_all(std::string str, const std::string& from, const std::string& to) {
      size_t start_pos = 0;
      while((start_pos = str.find(from, start_pos)) != std::string::npos) {
	str.replace(start_pos, from.length(), to);
	start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
      }
      return str;
    }

    //Place zeroes in front of all positive or negative signs
    std::string expression::add_implicit_zeroes(std::string str) {
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

    //Remove spaces from a string
    std::string expression::remove_spaces(std::string input) {
      input.erase(std::remove(input.begin(),input.end(),' '),input.end());
      return input;
    }

    //Remove rendundant parentheses around the given string
    std::string expression::remove_enclosing_parens(std::string str) {
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

    //Replace expressions with their actual function representation
    std::string expression::replace_functions(std::string str) {
      std::string r(str);
      for (int i = str.size() - 1; i >= 0; i--) {
	if (str[i] == '[') {
	  int j = i;
	  j--;
	  while ( j >= 0 &&
		  str[j] != '+' && str[j] != '-' &&
		  str[j] != '*' && str[j] != '/' &&
		  str[j] != ')' && str[j] != '(' &&
		  str[j] != ']' && str[j] != '[') {
	    j--;
	  }
	  j++;
	  std::string key = str.substr(j,i-j);
	  int k = i;
	  int bracketLevel = 0;
	  for (; k < int(str.size()); k++) {
	    if (str[k] == '[') {
	      bracketLevel++;
	    }
	    if (str[k] == ']') {
	      bracketLevel--;
	      if (bracketLevel == 0) {
		break;
	      }
	    }
	  }
	  int length = k-j+1;
	  std::string args = str.substr(i+1,k-i-1);
	  if (expression_table.find(key) == expression_table.end() && function_table.find(key) == function_table.end()) {
	    var_not_found_exception e(key);
	    e.append(" in expression table");
	    throw e;
	  } else if (function_table.find(key) == function_table.end()) {
	    if (expression_table.at(key).get_num_variables() != utils::split_string(args,",").size()) {
	      throw wrong_num_vars_exception(utils::split_string(args,"," ).size(), expression_table.at(key).get_num_variables());
	    }
	    str.replace(j,length,expression_table.at(key).change_variables(utils::split_string(args,",")));
	  }
	}
      }
      return str;
    }


    //Return true if the parentheses are matched
    bool expression::are_parens_matched(const std::string& str) {
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

    //Returns true if the given string is a function of an expression
    bool expression::is_function(const std::string& str) {
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



    //Return true if the string can be used as a mathematical expression
    bool expression::is_string_valid(const std::string& str) {
      if (str == "") {
	return false;
      } 
      if (!are_parens_matched(str)) {
	return false;
      }
      if (str[0] == '*' || str[0] == '/') {
	return false;
      }
      if (str[str.size()-1] == '+' || str[str.size()-1] == '-' || str[str.size()-1] == '*' || str[str.size()-1] == '/') {
	return false;
      }
      if (str.find("`") != std::string::npos ||
	  str.find("=") != std::string::npos ||
	  str.find("|") != std::string::npos ||
	  str.find("<") != std::string::npos ||
	  str.find(">") != std::string::npos ||
	  str.find("!") != std::string::npos ||
	  str.find("'") != std::string::npos ||
	  str.find(",") != std::string::npos ||
	  str.find("%") != std::string::npos ||
	  str.find("\"") != std::string::npos ||
	  str.find("\\") != std::string::npos ||
	  str.find("++") != std::string::npos ||
	  str.find("+-") != std::string::npos ||
	  str.find("+*") != std::string::npos ||
	  str.find("+/") != std::string::npos ||
	  str.find("-+") != std::string::npos ||
	  str.find("--") != std::string::npos ||
	  str.find("-*") != std::string::npos ||
	  str.find("-/") != std::string::npos ||
	  str.find("*+") != std::string::npos ||
	  str.find("*-") != std::string::npos ||
	  str.find("**") != std::string::npos ||
	  str.find("*/") != std::string::npos ||
	  str.find("/+") != std::string::npos ||
	  str.find("/-") != std::string::npos ||
	  str.find("/*") != std::string::npos ||
	  str.find("//") != std::string::npos ||
	  str.find("(*") != std::string::npos ||
	  str.find("(/") != std::string::npos ||
	  str.find("+)") != std::string::npos ||
	  str.find("-)") != std::string::npos ||
	  str.find("*)") != std::string::npos ||
	  str.find("/)") != std::string::npos ||
	  str.find("()") != std::string::npos ||
	  str.find(")(") != std::string::npos ||
	  str.find("+[") != std::string::npos ||
	  str.find("-[") != std::string::npos ||
	  str.find("*[") != std::string::npos ||
	  str.find("/[") != std::string::npos ||
	  str.find("^[") != std::string::npos ){
	return false;
      }
      for (std::size_t i = 1; i < str.size()-1; i++) {
	if (str[i] == '(') {
	  if (str[i-1] != '+' && str[i-1] != '-' && str[i-1] != '*' && str[i-1] != '/' && str[i-1] != '^' && str[i-1] != '(' && 
	      str[i-1] != ')' && str[i-1] != '[' && str[i-1] != ']') {
	    return false;
	  }
	} else if (str[i] == ')' || str[i] == ']') {
	  if (str[i+1] != '+' && str[i+1] != '-' && str[i+1] != '*'&& str[i+1] != '/' && str[i+1] != '^' && str[i+1] != '(' &&
	      str[i+1] != ')' && str[i+1] != '[' && str[i+1] != ']') {
	    return false;
	  }
	}
      }
      return true;
    }


    //========================
    //=== Key String methods ===
    //========================
    //searches for the last char, either key1 or key2, at the lowest parentheses level
    //Searches for two chars beacause some operators have the same priority
    int expression::search(const std::string& str,const std::size_t& parenSearchLevel,char key1, char key2)  {
      int index = -1;
      int count = 0;
      std::size_t parenLevel = 0;
      std::size_t bracketLevel = 0;
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

    //Find index of lowest priority operator
    //If no operators, return -1
    //If is a function, return -2
    int expression::find_lowest_priority(const std::string& str) {
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

    //====================
    //=== Tree Methods ===
    //==================== 
    //Load the given string into the binary tree
    void expression::load_tree(std::string str, int priorityOffset) {
      str = remove_enclosing_parens(str);
      int index = find_lowest_priority(str);
      if (index == -1) {
	insert({priorityOffset,str});
      } else if (index == -2) {
	std::string newStr( str.substr(str.find("[")+1, str.size()-2-str.find("[")) );
	std::string funcName(str.substr(0,str.find("[")));
	insert({priorityOffset,funcName});
	load_tree(newStr,priorityOffset+funcName.size());
      } else {
	std::string op;
	std::stringstream stream;
	stream << str[index];
	stream >> op;
	insert({priorityOffset+index,op});
	std::string substr1(str.substr(0,index));
	std::string substr2(str.substr(index+1,str.size()-index));
	load_tree(substr1,priorityOffset);
	load_tree(substr2,priorityOffset+index+1);
      }
    }

    //insert the given KeyString into the tree
    void expression::insert(KeyString item) {
      if (root == nullptr) {
	root = new Node;
	root->item = item;
	root->left = nullptr;
	root->right = nullptr;
      } else {
	_insert(item,root);
      }
    }

    //Insert helper method
    void expression::_insert(KeyString item, Node* node) {
      if (item.key <= node->item.key) {
	if (node->left != nullptr) {
	  _insert(item,node->left);
	} else {
	  node->left = new Node;
	  node->left->item = item;
	  node->left->left = nullptr;
	  node->left->right = nullptr;
	}
      } else {
	if (node->right != nullptr) {
	  _insert(item,node->right);
	} else {
	  node->right = new Node;
	  node->right->item = item;
	  node->right->left = nullptr;
	  node->right->right = nullptr;
	}
      }
    }

    //Recursively deallocate the tree
    void expression::destroy(Node* node) {
      if (node != nullptr) {
	destroy(node->left);
	destroy(node->right);
	delete node;
	node = nullptr;
      }
    }

    //Print the tree in order
    void expression::print_in_order() const {
      if (root == nullptr) {
	return;
      } else {
	_print_in_order(root);
      }
      std::cout << "\n";
    }

    //Helper for print in order
    void expression::_print_in_order(Node* node) const {
      if (node->left != nullptr) _print_in_order(node->left);
      std::cout << node->item.key << ":" << node->item.token << "\t";
      if (node->right != nullptr) _print_in_order(node->right);
    }

    //Helper for the evaluate method
    double expression::_evaluate(const std::vector<double>& x, Node* node) const {
      if (variable.size() > x.size()) {
	return 0;
      }
      if (utils::is_leaf(node)) {
	int index;
	if ( (index = utils::contains(node->item.token,variable)) != -1) {
	  return x[index];
	} else if (symbols_table.find(node->item.token) != symbols_table.end()) {
	  double val =  symbols_table.at(node->item.token);
	  return val;
	} else if (utils::is_string_num(node->item.token)) {
	  return atof(node->item.token.c_str());
	} else {
	  var_not_found_exception e(node->item.token);
	  throw e;
	}
      } else if(node->left == nullptr && node->right != nullptr) {
	if (function_table.find(node->item.token) != function_table.end()) {
	  return function_table.at(node->item.token)(_evaluate(x,node->right));
	} else {
	  return 0;
	}
      } else {
	double a = _evaluate(x,node->left);
	double b = _evaluate(x,node->right);
	std::string token = node->item.token;
	if(node->item.token == "+") {
	  return a + b;
	} else if (token == "-") {
	  return a -b;
	} else if (token == "*") {
	  return a*b;
	} else if (token == "/") {
	  return a/b;
	} else if (token == "^") {
	  return pow(a,b);
	} else {
	  return 0;
	}
      }
    }

    //True if the substring needs parentheses when being built into the new string
    bool expression::needs_parens(const std::string& str,const std::string& op,bool RHS) {
      if (is_function(str) || op =="+" || (op == "-" && !RHS)) {
	return false;
      } else if ( (op == "-" && RHS) || op == "*" || (op == "/" && !RHS)) {
	return utils::has_uncontained_op(str,'+') || utils::has_uncontained_op(str,'-');
      } else if ( (op == "/" && RHS) || (op == "^" && !RHS) ) {
	return (utils::has_uncontained_op(str,'+') || utils::has_uncontained_op(str,'-') ||
		utils::has_uncontained_op(str,'*') || utils::has_uncontained_op(str,'/'));
      } else if (op == "^" && RHS) {
	return(utils::has_uncontained_op(str,'+') || utils::has_uncontained_op(str,'-') ||
	       utils::has_uncontained_op(str,'*') || utils::has_uncontained_op(str,'/') || utils::has_uncontained_op(str,'^'));
      }
      return true;
    }
    
    //Appends str to all with parentheses if it needs them
    void expression::append_with_parens(std::string& all,const std::string& str,const std::string& op,bool RHS) {
      if (needs_parens(str,op,RHS)) {
	all.append("(").append(str).append(")");
      } else {
	all.append(str);
      }
    }

    //Helper for the build_expression method
    std::string expression::_build_expression(Node* node) {
      if (node->right == nullptr && node->left == nullptr) {
	return node->item.token;
      } if (node->left == nullptr && node->right != nullptr) {
	std::string str(node->item.token);
	str.append("[").append(_build_expression(node->right)).append("]");
	return str;
      }
      std::string str(node->item.token);
      std::string leftstr("");
      std::string rightstr("");
      if (node->left != nullptr) {
	leftstr = _build_expression(node->left);
      }
      if (node->right != nullptr) {
	rightstr = _build_expression(node->right);
      }
      std::string all("");
      //If addition
      if (str == "+") {
	if (leftstr!= "0") {
	  all.append(leftstr);
	  if (rightstr != "0") {
	    all.append(str).append(rightstr);
	  }
	} else {
	  all.append(rightstr);
	}
	//If subtraction
      } else if (str == "-") {
	if (leftstr != "0") {
	  all.append(leftstr);
	  if (rightstr != "0") {
	    append_with_parens(all.append(str),rightstr,str,true);
	  }
	} else {
	  append_with_parens(all.append("-"),rightstr,str,true);
	}
	//If multiplication
      } else if (str == "*") {
	if (leftstr == "0" || rightstr == "0") {
	  all.append("0");
	} else if (leftstr != "1") {
	  append_with_parens(all,leftstr,str,false);
	  if (rightstr != "1") {
	    append_with_parens(all.append(str),rightstr,str,true);
	  }
	} else {
	  append_with_parens(all,rightstr,str,false);
	}  
	//If division
      } else if (str == "/") {
	if ( leftstr == rightstr) {
	  all.append("1");
	} else if (leftstr == "0") {
	  all.append("0");
	} else  {
	  append_with_parens(all,leftstr,str,false);
	  if (rightstr != "1") {
	    append_with_parens(all.append(str),rightstr,str,true);
	  }
	}
	//If exponent 
      } else if (str == "^") {
	if (leftstr == "0") {
	  all.append("0");
	} else if (leftstr == "1" || rightstr == "0" || leftstr == rightstr) {
	  all.append("1");
	} else  {
	  append_with_parens(all,leftstr,str,false);
	  if (rightstr != "1") {
	    append_with_parens(all.append(str),rightstr,str,true);
	  }
	} 
      }
      return all;
    }

    //Change variable helper function
    void expression::_change_variable(Node* node, const std::string& from, const std::string& to) const {
      if (node == nullptr) {
	return;
      }
      if (node->left != nullptr) {
	_change_variable(node->left,from,to);
      }
      if (node->right != nullptr) {
	_change_variable(node->right,from,to);
      }
      if (node->item.token == from) node->item.token = to;
    }

    //Add a function to the expression Table
    void expression::add_function(const std::string& str, const expression& expr) {
      if (function_table.find(str) != function_table.end()) {
	throw protected_symbol_exception(str);
      }
      if (expression_table.find(str) != expression_table.end()) {
	expression_table[str]=expr;
      } else {
	expression_table.insert(std::make_pair(str, expr));
      }
    }

    //Add a constant to the symbols table
    void expression::add_constant(const std::string& str, double x) {
      if (symbols_table.find(str) != symbols_table.end()) {
	symbols_table[str] = x;
      } else {
	symbols_table.insert(std::make_pair(str,x));
      }
    }
    
    //Static swap function
    void expression::swap(expression& a, expression& b) {
      std::swap(a.expr,b.expr);
      std::swap(a.variable, b.variable);
      bool tmp = a.valid;
      a.valid = b.valid;
      b.valid = tmp;
      Node* tmp_ptr = a.root;
      a.root = b.root;
      b.root = tmp_ptr;
    }

    //Member swap function
    void expression::swap(expression& other) {
      swap(*this,other);
    }

    //============================
    //=== Static map variables ===
    //============================
    std::map<std::string, double> expression::symbols_table = {
      {"PI",atan(1)*4},
      {"e",2.71828182845904523536}};

    std::map<std::string, std::function<double(double)> > expression::function_table = {
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

    std::map<std::string,expression> expression::expression_table= { };

  }
}

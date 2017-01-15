#include "utils.hpp"
#include <algorithm>
#include <stack>

namespace Gold {
  namespace math {
    namespace utils {
      
      bool is_char_num(char ch) {
	return (ch >= '0' && ch <= '9');
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
	  all.append("(").append(str).append(")");
	} else {
	  all.append(str);
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
   
      bool is_leaf(node_ptr node) {
	return true;
      }
    
    }
  }
}

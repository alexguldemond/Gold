#include "utils.hpp"

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
	for (std::size_t i = 1; i < str.size(); i++) {
	  if (is_char_num(str[i])) continue;
	  if (str[i] == '.' && !oneDecimal) {
	    oneDecimal = true;
	    continue;
	  }
	  return false;
	}
	return true;
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
    
      //indexOf, returns -1 if not found
      int contains(const std::string& str, const std::vector<std::string>& vec) {
	int index = -1;
	for (std::size_t i = 0; i < vec.size(); i++) {
	  if (vec[i] == str) {
	    index = i;
	    break;
	  }
	}
	return index;
      }
    
      //True if the string has an uncontained operator
      bool has_uncontained_op(const std::string& str, char op) {
	int parenLevel = 0;
	for (std::size_t i = 0; i < str.size(); i++) {
	  if (str[i] == '(' || str[i] == '[') {
	    parenLevel--;
	  } else if (str[i] == ')' || str[i] == ']') {
	    parenLevel++;
	  } else if (parenLevel >= 0 && str[i] == op) {
	    return true;
	  }
	}
	return false;
      }
    
      //Returns true if a node is a leaf
      bool is_leaf(Node* node) {
	return (node->left == nullptr && node->right == nullptr);
      }
    }
  }
}

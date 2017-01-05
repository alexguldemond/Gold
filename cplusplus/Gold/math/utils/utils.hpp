#ifndef GOLD_MATH_UTILS
#define GOLD_MATH_UTILS

#if defined MATH
#define MATH_COMMON_DLLSPEC Q_DECL_EXPORT
#else
#define MATH_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

#include <string>
#include <vector>

namespace Gold {
  namespace math {
    namespace utils {
      
      struct KeyString {
	int key;
	std::string token;
      };

      struct Node {
	KeyString item;
	Node* right;
	Node* left;
      };

      bool is_char_num(char ch);
      bool is_string_num(const std::string& str);
      
      std::vector<std::string> split_string(std::string str, const std::string& delim);

      int contains(const std::string& str, const std::vector<std::string>& vec);

      bool has_uncontained_op(const std::string& str, char op);
      
      bool is_leaf(Node* node);

    }
  }
}

#endif

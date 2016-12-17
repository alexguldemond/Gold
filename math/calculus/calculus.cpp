#include <iostream>
#include "calculus.hpp"

namespace Gold {
  namespace math {
        std::map<std::string,std::string> calculus::d_table = {
      {"Sin","Cos[_]"},
      {"Cos","-Sin[_]"},
      {"Tan","Sec[_]^2"},
      {"Csc", "-Cot[_]*Csc[_]"},
      {"Sec", "Sec[_]*Tan[_]"},
      {"Cot", "-Csc[_]^2"},
      {"ArcSin", "1/(1-_^2)^(1/2)"},
      {"ArcCos", "-1/(1-_^2)^(1/2)"},
      {"ArcTan", "1/(1+_^2)"},
  
      {"Sinh","Cosh[_]"},
      {"Cosh", "Sinh[_]"},
      {"Tanh", "Cosh[_]"},
      {"ArcSinh", "1/(1+_^2)^(1/2)"},
      {"ArcCosh",  "1/(Sqrt[_-1]*Sqrt[_+1])"},
      {"ArcTanh", "1/(1-_^2)"},
  
      {"ln", "1/_"},
      {"Log", "1/(_*Log[10])"},
      {"Log2", "1/(_*Log[2])"},
      {"Sqrt","1/(2*Sqrt[_]"}
    };

    //===============================
    //====  calculus Operations  ====
    //===============================

    //Take the derrivative of an expression
    expression calculus::derrivative(const expression& expr, const std::string& var) {
      if (utils::contains(var,expr.variable) == -1) return expression("0",expr.variable);
      expression copy(expr);
      expression temp(_derrivative(copy.root,var),expr.get_variables());
      return expression(temp.build_expression(),expr.get_variables());
    }

    std::string calculus::_derrivative(Node* node, const std::string& var) {
      if (utils::is_leaf(node)) {
	return d_string(node->item.token,var);
      } else if (node->left == nullptr && node->right != nullptr) {
	std::string str = expression::remove_enclosing_parens(expression::_build_expression(node->right));
	std::string d = expression::remove_enclosing_parens(_derrivative(node->right,var));
	std::ostringstream stream;
	stream << "(" <<expression::replace_all(d_table.at(node->item.token),"_",str) << ")*(";
	stream << d << ")";
	return stream.str();
      } else {
	std::string op = node->item.token;
	std::string left_d = expression::remove_enclosing_parens(_derrivative(node->left,var));
	std::string right_d = expression::remove_enclosing_parens(_derrivative(node->right,var));
	std::string leftstr = expression::remove_enclosing_parens(expression::_build_expression(node->left));
	std::string rightstr =expression::remove_enclosing_parens(expression::_build_expression(node->right));
	std::string all("");
	if (op == "+" || op == "-") {
	  all.append("((").append(left_d).append(")").append(op).append("(").append(right_d).append("))");
	} else if (op == "*" || op == "/") {
	  std::string tmp;
	  std::string l("(");
	  l.append(left_d).append(")").append("*").append("(").append(rightstr).append(")");
	  std::string r("(");
	  r.append(leftstr).append(")").append("*").append("(").append(right_d).append(")");
	  if (op == "*") { 
	    tmp.append("(").append(l).append("+").append(r).append(")");
	  } else {
	    tmp.append("(").append(l).append("-").append(r).append(")").append("/").append("((");
	    tmp.append(rightstr).append(")^").append("2)");
	  }
	  all.append(tmp);
	} else {
	  std::ostringstream stream;
	  if (leftstr == var) {
	    if (utils::is_string_num(rightstr)) {
	      double val = atof(rightstr.c_str())-1;
	      stream << rightstr << "*" << leftstr << "^" << val;
	    } else {
	      stream << leftstr<< "^(" << rightstr << ")" << "*" 
		     << "(" << rightstr << "+" << leftstr << "*" << "ln[" << leftstr << "]" << "*(" << right_d <<"))/" << leftstr;
	    }
	  } else {
	    stream << "((" << leftstr << ")^(" << rightstr << "))" 
		   << "*((((" << rightstr << ")*(" << left_d << "))/(" << leftstr << "))+" 
		   << "(ln[" << leftstr << "]" << "*(" << right_d << ")))";
	  }
	  all.append(stream.str());
	}
	return all;
      }
    }

    inline std::string calculus::d_string(const std::string& str, const std::string& var) {
      return (str == var) ? "1" : "0";
    }
    
    

    double calculus::numerically_integrate(const expression& expr, double x1, double x2, double dx) {
      if (expr.get_num_variables() > 1) {
	wrong_num_vars_exception e(expr.get_num_variables(), 1);
	throw e;
      }
      auto abs = [](double x) -> double { return (x >= 0) ? x : -x ;};
      int n = static_cast<int>(abs(x2 - x1)/dx + 0.5);
      if (n & 1) n++; //If n is odd, make it even;
      dx = (x2 - x1)/n;
      std::cout << "dx = " << dx << "\n";
      double y0 = expr(x1);
      double yn = expr(x2);
      double sum = y0 + yn;
      for ( int i = 1; i < n-1; i++) {
	double yi = expr(x1 + i*dx);
	if (i & 1) {
	  sum += 2*yi;
	} else {
	  sum += 4*yi;
	}
      }
      return (dx/3)*sum;
    }

  }
}

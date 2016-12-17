#include <sstream>
#include <cstdlib>
#include "argparser.hpp"

namespace {  
  const int SPACES = 40;
  
  std::string _get_spaces(const std::string& str) {
    std::string spaces(" ");
    if (str.size() < SPACES) {
      for (std::size_t i = 0; i < SPACES - str.size(); i++) {
	spaces.append(" ");
      }
    }
    return spaces;
  }

 bool isCharNum(char ch) {
    return (ch >= '0' && ch <= '9');
  } 

  bool isStringNum(const std::string& str) {
    if (str == "") {
      return false;
    } else if (str.size() == 1 && !isCharNum(str[0])) {
      return false;
    } else if (str[0] != '-' && str[0] != '.' && !isCharNum(str[0])) {
      return false;
    }
    bool oneDecimal = false;
    for (std::size_t i = 1; i < str.size(); i++) {
      if (isCharNum(str[i])) continue;
      if (str[i] == '.' && !oneDecimal) {
	oneDecimal = true;
	continue;
      }
      return false;
    }
    return true;
  }
}

namespace Gold {

//=====================
//===  Constructor  ===
//=====================
argparser::argparser(std::size_t n,const std::string& m): n_pos_args(n), meta_arg(m) {
  opt_flags["h"] = false;
  long_short["help"] = "h";
  help_string = "Options:\n  --";
  help_string.append(_get_spaces("  --"));
  help_string.append("Ignore all arguments after this flag\n");
  add_opt_flag_to_help_string("h","help","Display this message");
}

//================================
//===  Private Helper Methods  ===
//================================
bool argparser::check_flags(const std::string& s,const std::string& l) {  
  if (s == "" || l == "") {
    return false;
  } else if (s[0] == '-' || l[0] == '-') {
    return false;
  } else if (isCharNum(s[0]) || isCharNum(s[0])) {
    return false;
  } else if (isStringNum(s) || isStringNum(l)) {
    return false;
  } else if (l.find("=") != std::string::npos) {
    return false;
  } else if (s.find(" ") != std::string::npos || l.find(" ") != std::string::npos) {
    return false;
  } else if (opt_args.find(s) != opt_args.end() || opt_flags.find(s) != opt_flags.end() || long_short.find(l) != long_short.end()) {
    return false;
  } else {
    return true;
  }
}

void argparser::add_opt_arg_to_help_string(const std::string& s, const std::string& l, const std::string& m, const std::string& h) { 
  std::ostringstream stream;
  stream << "  -" << s << " <--" << l << "=> " << m;
  std::string spaces = _get_spaces(stream.str());
  stream << spaces << h;
  if (detail_table[s].many) {
    stream << ". Can be set more than once";
  } 
  stream << "\n";
  help_string.append(stream.str());
}

void argparser::add_opt_flag_to_help_string(const std::string& s, const std::string& l, const std::string& h) {
  std::ostringstream stream;
  stream << "  -" << s << " <--" << l << ">";
  std::string spaces = _get_spaces(stream.str());
  stream << spaces << h << "\n";
  help_string.append(stream.str());
}

void argparser::finalize_help_string(const std::string& toolName) {
  std::ostringstream stream;
  std::string tool(toolName);
  while (tool.find("/") != std::string::npos) {
    tool = tool.substr(tool.find("/")+1,tool.size()-tool.find("/")-1);
  }
  stream << "\nUsage: " << tool <<" [options <args>]";
  for (std::size_t i = 0; i < n_pos_args; i++) {
    stream << " " << meta_arg << i+1;
  }
  stream << "...\n" << help_string << "\n";
  std::string newString = stream.str();
  help_string = newString;
}

void argparser::insert_long_form(const std::string& l) {
  std::string arg = l.substr(2,l.size()-2);
  if (arg.find("=") == std::string::npos) {     //If in flag form
    if (long_short.find(arg) == long_short.end()) {
      std::ostringstream stream;
      stream << "Error: unrecognized option " << l << "\n";
      throw stream.str();
    }
    std::string s = long_short.at(arg);
    if (opt_args.find(s) != opt_args.end()) {      //If is supposed to be given an argument
      std::ostringstream stream;
      stream << "Give option " << arg << " an argument with " << l << "=<arg>\n";
      throw stream.str();
    } else if (opt_flags.find(s) != opt_flags.end()) { //If is a flag
      if (!detail_table[s].isSet) {
	opt_flags[s] = !opt_flags[s];  //Switch it from default
	detail_table[s].isSet = true;
      }
    } else {                                           //If flag not found
      std::ostringstream stream;
      stream << "Error: unrecognized option " << l << "\n";
      throw stream.str();
    }
  } else {
    std::string flag = arg.substr(0,arg.find("="));
    arg = arg.substr(arg.find("=")+1, arg.size()-arg.find("=")-1);
    if (long_short.find(flag) == long_short.end()) {
      std::ostringstream stream;
      stream << "Error: unrecognized option " << l << "\n";
      throw stream.str();
    } else {
      std::string s = long_short.at(flag);
      if (!detail_table[s].isSet) {
	opt_args[s] = arg;
	detail_table[s].isSet = true;
      } else {
	if (!detail_table[s].many) {
	  std::ostringstream stream;
	  stream << "Error: " << "-" << s << "/" << "--" << flag << " can only be set once\n";
	  throw stream.str();
	} else {
	  opt_args[s].append(" ");
	  opt_args[s].append(arg);
	}
      }
    }
  }
}

void argparser::insert_short_form(const std::string& s) {
  std::string arg = s.substr(1,s.size()-1);
  if (!detail_table[arg].isSet) {
    opt_flags[arg] = !opt_flags[arg];
    detail_table[arg].isSet = true;
  }
}

void argparser::insert_short_form(const std::string& s, const std::string& arg) {
  std::string flag = s.substr(1,s.size()-1);
  if (!detail_table[flag].isSet) {
    opt_args[flag] = arg;
    detail_table[flag].isSet = true;
  } else {
    if (!detail_table[flag].many) {
      std::ostringstream stream;
      stream << "Error: " << "-" << flag << " can only be set once\n";
      throw stream.str();
    } else {
      opt_args[flag].append(" ");
      opt_args[flag].append(arg);
    }
  }
} 
//==========================
//===  Public functions  ===
//==========================
void argparser::add_opt_arg(const std::string& shortFlag, const std::string& longFlag, const std::string& defaultVal,
			  const std::string& metavar, const std::string& help,bool more_than_once) {
  if (check_flags(shortFlag, longFlag)) {
    opt_args[shortFlag] = defaultVal;
    long_short[longFlag] = shortFlag;
    Details e;
    e.isSet = false;
    e.many = more_than_once;
    detail_table[shortFlag] = e;
    add_opt_arg_to_help_string(shortFlag, longFlag, metavar, help);
  }
}

void argparser::add_opt_flag(const std::string& shortFlag, const std::string& longFlag, bool defaultVal, const std::string& help) {
  if (check_flags(shortFlag, longFlag)) {
    add_opt_flag_to_help_string(shortFlag, longFlag, help);
    opt_flags[shortFlag] = defaultVal;
    long_short[longFlag] = shortFlag;
    Details e;
    e.isSet = false;
    e.many = false;
    detail_table[shortFlag] = e;
  }
}

template <>
int argparser::get_pos_arg(std::size_t index) {
  if (index >= pos_args.size()) {
    return -1;
  } else {
    return atoi(pos_args[index].c_str());
  }
}

template <>
double argparser::get_pos_arg(std::size_t index) {
  if (index >= pos_args.size()) {
    return -1.0;
  } else {
    return atof(pos_args[index].c_str());
  }
}

template <>
std::string argparser::get_pos_arg(std::size_t index) {
  if (index >= pos_args.size()) {
    return "";
  } else {
    return pos_args[index];
  }
}

template <> 
int argparser::get_opt_arg(const std::string& shortForm) {
  if (opt_args.find(shortForm) == opt_args.end()) {
    return -1;
  } else {
    return atoi(opt_args.at(shortForm).c_str());
  }
}

template <>
double argparser::get_opt_arg(const std::string& shortForm) {
  if (opt_args.find(shortForm) == opt_args.end()) {
    return -1.0;
  } else {
    return atof(opt_args.at(shortForm).c_str());
  }
}

template <>
std::string argparser::get_opt_arg(const std::string& shortForm) {
  if (opt_args.find(shortForm) == opt_args.end()) {
    return "";
  } else {
    return opt_args.at(shortForm);
  }
}

bool argparser::get_opt_flag(const std::string& shortForm) {
  if (opt_flags.find(shortForm) == opt_flags.end()) {
    return false;
  } else {
    return opt_flags.at(shortForm);
  }
}

void argparser::parse_args(int argc, char** argv) {
  finalize_help_string(std::string(argv[0]));
  for (int i = 1; i < argc; i++) {
    std::string current(argv[i]);
    if (current == "-h" || current == "--help") {
      throw help_string;
    } else if (current == "--") {
      break;
    } else if (current[0] != '-' || isStringNum(current) || current.size() == 1) {
      pos_args.push_back(current);
    } else if (current.find("--") == 0) {
      insert_long_form(current);
    } else {
      std::string flag = current.substr(1,current.size()-1);
      if (opt_args.find(flag) == opt_args.end() && opt_flags.find(flag) == opt_flags.end()) {
	std::ostringstream stream;
	stream << "Error: unrecognized option " << current << "\n";
	throw stream.str();
      } else if (opt_args.find(flag) != opt_args.end()) {
	insert_short_form(current, std::string(argv[++i]));
      } else {
	insert_short_form(current);
      }
    }
  }
  if (pos_args.size() < n_pos_args) {
    std::ostringstream stream;
    stream << "Error: this tool takes at least " << n_pos_args << " arguments, only " << pos_args.size() << " given.\n";
    throw stream.str();
  }
}

}

#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>

#if defined ARGPARSER
#define ARGPARSER_COMMON_DLLSPEC Q_DECL_EXPORT
#else
#define ARGPARSER_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

struct Details {
  bool isSet;
  bool many;
};
  
namespace Gold {
  class argparser {
  public:
    argparser(std::size_t n = 0,const std::string& m = "arg");
    virtual ~argparser() { }
    virtual void add_opt_arg(const std::string& shortFlag,const std::string& longFlag,const std::string& defaultVal,
			     const std::string& metaVar,const std::string& help,bool more_than_once = false);
    virtual void add_opt_flag(const std::string& shortFlag,const std::string& longFlag, bool defaultVal,const std::string& help);
    
    virtual std::size_t get_num_pos_args() { return pos_args.size(); }
    virtual std::size_t get_min_pos_args() { return n_pos_args; }
    template <typename T> T get_pos_arg(std::size_t index);
    template <typename T> T get_opt_arg(const std::string& shortForm);
    virtual bool get_opt_flag(const std::string& shortForm);
    
    virtual void parse_args(int argc, char** argv);
    
  private:
    argparser(const argparser& other);                  //Uncopyable
    argparser& operator=(const argparser& other);       //Uncopyable
    
    virtual bool check_flags(const std::string& s,const std::string& l);
    virtual void add_opt_arg_to_help_string(const std::string& s, const std::string& l, const std::string& m, const std::string& h);
    virtual void add_opt_flag_to_help_string(const std::string& s, const std::string& l, const std::string& h);
    virtual void finalize_help_string(const std::string& toolName);
    virtual void insert_long_form(const std::string& l);
    virtual void insert_short_form(const std::string& s);
    virtual void insert_short_form(const std::string& s, const std::string& arg);
    
    std::string help_string;
    std::size_t n_pos_args;
    std::string meta_arg;
    std::vector<std::string> pos_args;
    std::map<std::string,std::string> opt_args;
    std::map<std::string,bool> opt_flags;
    std::map<std::string, std::string> long_short;
    std::map<std::string,Details> detail_table;
    
  };
}
#endif

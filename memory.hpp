#ifndef SCRIPT_MEMORY_HPP
#define SCRIPT_MEMORY_HPP

#include "utils.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <string_view>

#include <iostream>

namespace script::memory {

using register_addr = char;
using symbol = std::pair<register_addr, int>;
using symbol_table = std::map<std::string_view, symbol>;

class memory {
public:
  void define(std::string_view name, int value);
  symbol resolve(std::string_view name) const;
  void redefine(std::string_view name, int value);

private:
  symbol_table memory_;
  char get_var_reg() const;
};

char memory::get_var_reg() const {
  return static_cast<char>(-4 * (memory_.size() + 1));
}

void memory::redefine(std::string_view name, int value) {
  auto ret = memory_.find(name);

  if (ret == memory_.end())
    throw "variable not found";

  char addr = std::get<char>(memory_[name]);
  memory_[name] = std::make_pair(addr, value);
}

void memory::define(std::string_view name, int value) {
  auto ret = memory_.find(name);

  if (ret == memory_.end()) {
    auto ret2 = memory_.insert({name, {get_var_reg(), value}});

    if (!ret2.second)
      throw "failed define sym";
  }

  char addr = std::get<char>(memory_[name]);

  memory_[name] = std::make_pair(addr, value);
}

symbol memory::resolve(std::string_view name) const {
  auto ret = memory_.find(name);

  if (ret == memory_.end())
    throw "variable not found";

  return ret->second;
}

} // namespace script::memory

#endif
#ifndef SCRIPT_LEXER_HPP
#define SCRIPT_LEXER_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

#include <iostream>

namespace script::lexer {

template <char> struct C {};

struct number {
  int value;
};
struct identifier {
  std::string_view name;
};

struct eof {};

struct while_t {};
struct print_t {};
struct return_t {};
struct function_t {};
struct dec_t {};

using token =
    std::variant<std::monostate, number, C<','>, C<'+'>, C<'-'>, C<'*'>, C<'/'>, C<'('>,
                 C<')'>, C<';'>, C<'='>, C<'{'>, C<'}'>, identifier, while_t,
                 print_t, return_t, function_t, dec_t, eof>;

class lexer {
public:
  explicit lexer(std::string buffer)
      : buffer_{std::move(buffer)}, cursor_{buffer_.c_str()},
        end_{cursor_ + buffer_.length()} {}

  token get_current_token() const;
  void next_token();
  token peek();
  void drop();
  template <typename T> void match();

private:
  std::string buffer_;
  const char *cursor_;
  const char *end_;
  token current_token_;

  token scan_identifier_();
  token scan_number_();
  token get_next_token_();

  void skip_whitespaces_();
};

token lexer::get_current_token() const { return current_token_; }

void lexer::next_token() {
  drop();
  peek();
}

token lexer::peek() {
  if (std::holds_alternative<std::monostate>(current_token_))
    current_token_ = get_next_token_();
  return current_token_;
}

void lexer::drop() { current_token_ = std::monostate{}; }

template <typename T> void lexer::match() {
  if (std::get_if<T>(&current_token_)) {
    drop();
    peek();
  } else {
    throw "match failed";
  }
}

token lexer::scan_identifier_() {
  auto start_ident = cursor_++;

  while (cursor_ != end_ && (std::isalnum(*cursor_) || *cursor_ == '_'))
    ++cursor_;

  std::string_view result{start_ident, static_cast<std::size_t>(std::distance(
                                           start_ident, cursor_))};

  if (result == "while")
    return {while_t{}};
  else if (result == "print")
    return {print_t{}};
  else if (result == "return")
    return {return_t{}};
  else if(result == "dec")
    return {dec_t{}};
  else
    return {identifier{result}};
}

token lexer::scan_number_() {
  std::string tmp;

  while (std::isdigit(*cursor_))
    tmp += *cursor_++;

  number ret{std::stoi(tmp)};
  return {ret};
}

token lexer::get_next_token_() {
  skip_whitespaces_();

  if (cursor_ >= end_)
    return {eof{}};

  if (std::isalpha(*cursor_))
    return scan_identifier_();

  switch (*cursor_) {
  case '+':
    ++cursor_;
    return {C<'+'>{}};
  case '-':
    ++cursor_;
    return {C<'-'>{}};
  case '*':
    ++cursor_;
    return {C<'*'>{}};
  case '/':
    ++cursor_;
    return {C<'/'>{}};
  case '(':
    ++cursor_;
    return {C<'('>{}};
  case ')':
    ++cursor_;
    return {C<')'>{}};
  case ';':
    ++cursor_;
    return {C<';'>{}};
  case '=':
    ++cursor_;
    return {C<'='>{}};
  case '{':
    ++cursor_;
    return {C<'{'>{}};
  case '}':
    ++cursor_;
    return {C<'}'>{}};
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return scan_number_();
  }
  throw "unknown token";
}

void lexer::skip_whitespaces_() {
  if (std::isspace(*cursor_))
    ++cursor_;
}

} // namespace script::lexer

#endif
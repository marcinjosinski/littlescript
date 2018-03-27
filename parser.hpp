#ifndef SCRIPT_PARSER_HPP
#define SCRIPT_PARSER_HPP

#include "assembler.hpp"
#include "lexer.hpp"
#include "memory.hpp"
#include "utils.hpp"

#include <memory>
#include <string>
#include <variant>

namespace script::parser {

class parser {
public:
  char *generate_function(std::string code);

private:
  std::unique_ptr<lexer::lexer> lexer_;
  memory::memory memory_;
  assembler::function function_{space_size{1024}};
  int parse_(std::string s);

  void function_definition();
  void decrement_statement();
  void statement();
  void while_statement();
  void return_statement();
  void assign_statement();

  int expression();
  int term();
  int factor();
  int parse_paren();
};

void parser::decrement_statement() {
  lexer_->match<lexer::dec_t>();
  auto identifier = std::visit(
      overload{
          [=](lexer::identifier identifier) { return identifier.name; },
          [=](auto) -> std::string_view { throw "expected identifier name"; }},
      lexer_->get_current_token());
  lexer_->drop();
  lexer_->peek();

  auto ident_addr = std::get<char>(memory_.resolve(identifier));

  function_.emit(0x83);
  function_.emit(0x6d);
  function_.emit(ident_addr);
  function_.emit(0x01);

  memory_.redefine(identifier, std::get<int>(memory_.resolve(identifier)) - 1);
  lexer_->match<lexer::C<';'>>();
}

void parser::function_definition() {

  // oraz dodawanie parametrow do tablicy symboli
  // tutaj trzeba dodac assembly dodawania parametrow

  // function identifier(param1, param2) {}

  lexer_->match<lexer::function_t>();

  auto func = std::visit(
      overload{[=](lexer::identifier func) { return func.name; },
               [=](auto) -> std::string_view { throw "expected func name"; }},
      lexer_->get_current_token());
  lexer_->drop();
  lexer_->peek();

  lexer_->match<lexer::C<'('>>();

  for (bool done = false; !false;) {
    std::visit(
        overload{[=](lexer::identifier param) {
                   /* tutaj dodaj param do stosu  matchuj C<','> i od nowa */
                 },
                 [&](lexer::C<')'>) { done = true; },
                 [=](auto) -> void { throw "failed function"; }},
        lexer_->get_current_token());
  }
  lexer_->drop();
  lexer_->peek();

  lexer_->match<lexer::C<')'>>();

  lexer_->match<lexer::C<'{'>>();

  for (bool done = false; !false;) {
    std::visit(overload{[&](lexer::C<'}'>) { done = true; },
                        [=](auto) -> void { statement(); }},
               lexer_->get_current_token());
  }

  lexer_->drop();
  lexer_->peek();
}

// done
void parser::while_statement() {

  lexer_->match<lexer::while_t>();
  lexer_->match<lexer::C<'('>>();

  auto ident = std::get<lexer::identifier>(lexer_->get_current_token());
  std::cout << "ok1\n";
  lexer_->match<lexer::identifier>();
  std::cout << "ok2\n";

  lexer_->match<lexer::C<')'>>();
  lexer_->match<lexer::C<'{'>>();

  // cmp var, 00
  function_.emit(0x83);
  function_.emit(0x7d);
  function_.emit(std::get<char>(memory_.resolve(ident.name)));
  function_.emit(0x00);

  // je
  function_.emit(0x74);
  function_.emit(0x06);

  // statements

  for (bool done = false; !done;) {
    std::visit(overload{[&](lexer::C<'}'>) { done = true; },
                        [=](auto) -> void { statement(); }},
               lexer_->get_current_token());
  }

  function_.emit(0xeb);
  function_.emit(0xf4);

  lexer_->match<lexer::C<'}'>>();
}

void parser::return_statement() {

  lexer_->match<lexer::return_t>();

  auto return_expression = [&]() {
    function_.emit(0xb8); // mov to eax
    function_.emit_int(expression());
  };

  auto return_identifier = [&](lexer::identifier i) {
    function_.emit(0x8b);
    function_.emit(0x45);
    function_.emit(std::get<char>(memory_.resolve(i.name)));
    lexer_->drop();
    lexer_->peek();
  };

  std::visit(
      overload{[=](lexer::identifier i) { return_identifier(i); },
               [=](lexer::number) { return_expression(); },
               [=](lexer::C<'+'>) { return_expression(); },
               [=](lexer::C<'-'>) { return_expression(); },
               [=](auto) -> void { throw "invalid token return statement"; }},
      lexer_->get_current_token());

  lexer_->match<lexer::C<';'>>();
}

void parser::assign_statement() {
  auto token = lexer_->get_current_token();

  auto variable = std::visit(
      overload{[=](lexer::identifier var) { return var.name; },
               [=](auto) -> std::string_view { throw "expected var name"; }},
      token);

  lexer_->match<lexer::identifier>();
  lexer_->match<lexer::C<'='>>();

  memory_.define(variable, expression());

  lexer_->match<lexer::C<';'>>();

  function_.emit(0xc7);                                         // mov
  function_.emit(0x45);                                         // rbp
  function_.emit(std::get<char>(memory_.resolve(variable)));    // - ile
  function_.emit_int(std::get<int>(memory_.resolve(variable))); // int
}

void parser::statement() {
  return std::visit(overload{[=](lexer::return_t) { return_statement(); },
                             [&](auto) { assign_statement(); },
                             [=](lexer::dec_t) { decrement_statement(); },
                             [=](lexer::while_t) { while_statement(); }},
                    lexer_->get_current_token());
}

char *parser::generate_function(std::string code) {
  parse_(std::move(code));

  return function_.get_code();
}

int parser::parse_(std::string s) {
  lexer_ = std::make_unique<lexer::lexer>(std::move(s));

  auto token = lexer_->peek();

  // tutaj po prostu petla

  statement();
  statement();
  statement();
  statement();
}

int parser::expression() {
  auto result = term();

  for (bool done = false; !done;) {
    auto token = lexer_->get_current_token();

    result = std::visit(overload{[=](lexer::C<'+'>) {
                                   lexer_->next_token();
                                   return result + term();
                                 },
                                 [=](lexer::C<'-'>) {
                                   lexer_->next_token();
                                   return result - term();
                                 },
                                 [&](auto) -> int {
                                   done = true;
                                   return result;
                                 }},
                        token);
  }
  return result;
}

int parser::term() {
  auto result = factor();

  for (bool done = false; !done;) {
    auto token = lexer_->get_current_token();

    result = std::visit(overload{[=](lexer::C<'*'>) {
                                   lexer_->next_token();
                                   return result * factor();
                                 },
                                 [=](lexer::C<'/'>) {
                                   lexer_->next_token();
                                   return result / factor();
                                 },
                                 [&](auto) {
                                   done = true;
                                   return result;
                                 }},
                        token);
  }
  return result;
}

int parser::factor() {
  auto token = lexer_->get_current_token();

  lexer_->next_token();

  return std::visit(overload{[=](lexer::number n) { return n.value; },
                             [=](lexer::C<'('>) { return parse_paren(); },
                             [=](lexer::C<'+'>) { return term(); },
                             [=](lexer::C<'-'>) { return -term(); },
                             [=](lexer::identifier var) {
                               return std::get<int>(memory_.resolve(var.name));
                             },
                             [=](auto) -> int { throw "unexpected token"; }},
                    token);
}

int parser::parse_paren() {
  auto result = expression();
  auto token = lexer_->get_current_token();

  lexer_->match<lexer::C<')'>>();

  return result;
}

} // namespace script::parser

#endif

#include "parser.hpp"
#include <iostream>
#include <string>

int main() {

  script::parser::parser p;

  // TODO:
  // FUNCTION_DECLARATION WITH PASSING PARAMETERS
  // FIX EXPRESSIONS (term, factor)

  std::string code = ""
                     "ident = 3;"
                     "while (ident) {"
                     "dec ident;"
                     "}"
                     "ident = ident + 3;"
                     "return ident;"
                     "";

  try {

    typedef int (*func)(void);
    func f = reinterpret_cast<func>(p.generate_function(code));

    std::cout << f() << "\n";

  } catch (const char *err) {
    std::cout << "oops: " << err;
  }
}
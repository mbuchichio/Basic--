#pragma once

#include <string>
#include <vector>

#include "token.hpp"

namespace basicpp::frontend::ast {

struct literal {
    token_kind kind;
    std::string lexeme;
};

struct const_decl {
    std::string name;
    literal value;
};

struct import_decl {
    std::string path;
};

struct module_decl {
    std::string name;
    std::vector<import_decl> imports;
    std::vector<const_decl> constants;
};

} // namespace basicpp::frontend::ast

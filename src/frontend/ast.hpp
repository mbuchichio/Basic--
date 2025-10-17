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

struct state_transition {
    std::string event;
    std::string target_state;
};

struct state_decl {
    std::string name;
    std::string initial_state;
    std::vector<state_transition> transitions;
};

struct import_decl {
    std::string path;
};

struct module_decl {
    std::string name;
    std::vector<import_decl> imports;
    std::vector<const_decl> constants;
    std::vector<state_decl> states;
};

} // namespace basicpp::frontend::ast

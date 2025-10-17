#pragma once

#include <optional>
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

struct command_decl {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<token> body_tokens;
};

struct function_decl {
    std::string name;
    std::vector<std::string> parameters;
    std::optional<std::string> return_type;
    std::vector<token> body_tokens;
};

struct import_decl {
    std::string path;
};

struct module_decl {
    std::string name;
    std::vector<import_decl> imports;
    std::vector<const_decl> constants;
    std::vector<state_decl> states;
    std::vector<command_decl> commands;
    std::vector<function_decl> functions;
};

} // namespace basicpp::frontend::ast

#pragma once

#include <vector>

#include <basicpp/core/result.hpp>

#include "ast.hpp"
#include "token.hpp"

namespace basicpp::frontend {

class parser {
public:
    static core::result<ast::module_decl, std::string> parse_module(const std::vector<token>& tokens);
};

} // namespace basicpp::frontend

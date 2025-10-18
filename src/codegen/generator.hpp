#pragma once

#include <string>

#include <basicpp/core/result.hpp>

#include "frontend/ast.hpp"

namespace basicpp::codegen {

core::result<std::string, std::string> generate_translation_unit(const frontend::ast::module_decl& module);

} // namespace basicpp::codegen

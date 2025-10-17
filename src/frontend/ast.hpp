#pragma once

#include <string>
#include <vector>

namespace basicpp::frontend::ast {

struct import_decl {
    std::string path;
};

struct module_decl {
    std::string name;
    std::vector<import_decl> imports;
};

} // namespace basicpp::frontend::ast

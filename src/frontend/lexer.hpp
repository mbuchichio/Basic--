#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <basicpp/core/result.hpp>

#include "token.hpp"

namespace basicpp::frontend {

class lexer {
public:
    static core::result<std::vector<token>, std::string> tokenize(std::string_view source);
};

} // namespace basicpp::frontend

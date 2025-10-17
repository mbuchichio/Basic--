#include <stdexcept>
#include <string>

#include <basicpp/testing/selftest.hpp>

#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"

namespace {

using basicpp::frontend::lexer;
using basicpp::frontend::parser;
} // namespace

BASICPP_TEST(ParserParsesModuleName) {
    const std::string source = "module App\n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    if (module.value().name != "App") {
        throw std::runtime_error("unexpected module name");
    }
}

BASICPP_TEST(ParserRejectsMissingModuleKeyword) {
    const std::string source = "function Main()\nend function\n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (module) {
        throw std::runtime_error("parser should have failed");
    }

    if (module.error() != "expected 'module' keyword") {
        throw std::runtime_error("unexpected parser error message");
    }
}

BASICPP_TEST_MAIN()

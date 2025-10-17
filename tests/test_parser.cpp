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

BASICPP_TEST(ParserParsesImportList) {
    const std::string source = "module App\nimport Basicpp.Command\nimport System.Timer\n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    const auto& imports = module.value().imports;
    if (imports.size() != 2) {
        throw std::runtime_error("unexpected import count");
    }

    if (imports[0].path != "Basicpp.Command") {
        throw std::runtime_error("unexpected first import path");
    }

    if (imports[1].path != "System.Timer") {
        throw std::runtime_error("unexpected second import path");
    }
}

BASICPP_TEST(ParserRejectsBrokenImport) {
    const std::string source = "module App\nimport\n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (module) {
        throw std::runtime_error("parser should have failed");
    }

    if (module.error() != "expected identifier after 'import'") {
        throw std::runtime_error("unexpected parser error message");
    }
}

BASICPP_TEST_MAIN()

#include <stdexcept>
#include <string>

#include <basicpp/testing/selftest.hpp>

#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"
#include "frontend/token.hpp"

namespace {

using basicpp::frontend::lexer;
using basicpp::frontend::parser;
using basicpp::frontend::token_kind;
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

BASICPP_TEST(ParserParsesConstLiteral) {
    const std::string source = "module App\nconst Version = \"0.1.0\"\n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    const auto& constants = module.value().constants;
    if (constants.size() != 1) {
        throw std::runtime_error("unexpected constant count");
    }

    if (constants[0].name != "Version") {
        throw std::runtime_error("unexpected constant name");
    }

    if (constants[0].value.kind != token_kind::string_literal) {
        throw std::runtime_error("unexpected literal kind");
    }

    if (constants[0].value.lexeme != "0.1.0") {
        throw std::runtime_error("unexpected literal value");
    }
}

BASICPP_TEST(ParserRejectsConstMissingLiteral) {
    const std::string source = "module App\nconst Version = \n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (module) {
        throw std::runtime_error("parser should have failed");
    }

    if (module.error().find("expected literal after '='") == std::string::npos) {
        throw std::runtime_error("unexpected parser error message");
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

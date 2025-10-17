#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <basicpp/testing/selftest.hpp>

#include "frontend/lexer.hpp"

namespace {

using basicpp::frontend::lexer;
using basicpp::frontend::token;
using basicpp::frontend::token_kind;

void expect_token(const token& tok, token_kind kind, std::string_view lexeme) {
    if (tok.kind != kind) {
        throw std::runtime_error("unexpected token kind");
    }
    if (tok.lexeme != lexeme) {
        throw std::runtime_error("unexpected token lexeme");
    }
}

} // namespace

BASICPP_TEST(LexerParsesModuleHeader) {
    const std::string source = "module Main\n";
    auto result = lexer::tokenize(source);
    if (!result) {
        throw std::runtime_error("lexer should succeed");
    }

    const auto& tokens = result.value();
    if (tokens.size() != 3) {
        throw std::runtime_error("unexpected token count");
    }

    expect_token(tokens[0], token_kind::keyword_module, "module");
    expect_token(tokens[1], token_kind::identifier, "Main");
    expect_token(tokens[2], token_kind::end_of_file, "");
}

BASICPP_TEST(LexerRecognisesStringLiteral) {
    const std::string source = "command Say\n    print \"hi\"\nend command\n";
    auto result = lexer::tokenize(source);
    if (!result) {
        throw std::runtime_error("lexer should succeed");
    }

    const auto& tokens = result.value();
    bool found_string = false;
    for (const auto& tok : tokens) {
        if (tok.kind == token_kind::string_literal && tok.lexeme == "hi") {
            found_string = true;
            break;
        }
    }

    if (!found_string) {
        throw std::runtime_error("string literal token not found");
    }
}

BASICPP_TEST(LexerErrorsOnUnterminatedString) {
    const std::string source = "const Greeting = \"hola";
    auto result = lexer::tokenize(source);
    if (result) {
        throw std::runtime_error("lexer should have failed");
    }

    if (result.error() != "unterminated string literal") {
        throw std::runtime_error("unexpected error message");
    }
}

BASICPP_TEST(LexerSkipsComments) {
    const std::string source = "// comment\nlet x = 42\n";
    auto result = lexer::tokenize(source);
    if (!result) {
        throw std::runtime_error("lexer should succeed");
    }

    const auto& tokens = result.value();
    // Expected tokens: let, identifier, assignment, integer, eof
    if (tokens.size() != 5) {
        throw std::runtime_error("unexpected token count after comment");
    }

    expect_token(tokens[0], token_kind::keyword_let, "let");
    expect_token(tokens[1], token_kind::identifier, "x");
    expect_token(tokens[2], token_kind::assignment, "=");
    expect_token(tokens[3], token_kind::integer_literal, "42");
    expect_token(tokens[4], token_kind::end_of_file, "");
}

BASICPP_TEST_MAIN()

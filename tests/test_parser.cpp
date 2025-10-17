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

BASICPP_TEST(ParserParsesStateMachine) {
    const std::string source =
        "module App\n"
        "state AppState = Idle\n"
        "on Start => Running\n"
        "on Stop => Idle\n";

    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    const auto& states = module.value().states;
    if (states.size() != 1) {
        throw std::runtime_error("unexpected state count");
    }

    if (states[0].name != "AppState") {
        throw std::runtime_error("unexpected state name");
    }

    if (states[0].initial_state != "Idle") {
        throw std::runtime_error("unexpected initial state");
    }

    if (states[0].transitions.size() != 2) {
        throw std::runtime_error("unexpected transition count");
    }

    if (states[0].transitions[0].event != "Start" || states[0].transitions[0].target_state != "Running") {
        throw std::runtime_error("unexpected first transition");
    }

    if (states[0].transitions[1].event != "Stop" || states[0].transitions[1].target_state != "Idle") {
        throw std::runtime_error("unexpected second transition");
    }
}

BASICPP_TEST(ParserRejectsStateWithoutTransition) {
    const std::string source = "module App\nstate AppState = Idle\n";
    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (module) {
        throw std::runtime_error("parser should have failed");
    }

    if (module.error() != "state requires at least one 'on' transition") {
        throw std::runtime_error("unexpected parser error message");
    }
}

BASICPP_TEST(ParserParsesCommandWithParameters) {
    const std::string source =
        "module App\n"
        "command SayHello(name, context)\n"
        "return\n"
        "end command\n";

    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    const auto& commands = module.value().commands;
    if (commands.size() != 1) {
        throw std::runtime_error("unexpected command count");
    }

    if (commands[0].name != "SayHello") {
        throw std::runtime_error("unexpected command name");
    }

    if (commands[0].parameters.size() != 2) {
        throw std::runtime_error("unexpected parameter count");
    }

    if (commands[0].parameters[0] != "name" || commands[0].parameters[1] != "context") {
        throw std::runtime_error("unexpected parameter names");
    }

    if (commands[0].body_tokens.empty()) {
        throw std::runtime_error("expected command body tokens");
    }

    if (commands[0].body_tokens.front().kind != token_kind::keyword_return) {
        throw std::runtime_error("unexpected first body token");
    }
}

BASICPP_TEST(ParserRejectsCommandWithoutEnd) {
    const std::string source =
        "module App\n"
        "command SayHello()\n"
        "return\n";

    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (module) {
        throw std::runtime_error("parser should have failed");
    }

    if (module.error() != "expected 'end command' before end of input") {
        throw std::runtime_error("unexpected parser error message");
    }
}

BASICPP_TEST(ParserParsesFunctionWithReturnType) {
    const std::string source =
        "module App\n"
        "function Main(argv) as Result.App\n"
        "return\n"
        "end function\n";

    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    const auto& functions = module.value().functions;
    if (functions.size() != 1) {
        throw std::runtime_error("unexpected function count");
    }

    if (functions[0].name != "Main") {
        throw std::runtime_error("unexpected function name");
    }

    if (!functions[0].return_type || functions[0].return_type.value() != "Result.App") {
        throw std::runtime_error("unexpected return type");
    }

    if (functions[0].parameters.size() != 1 || functions[0].parameters[0] != "argv") {
        throw std::runtime_error("unexpected function parameters");
    }

    if (functions[0].body_tokens.empty()) {
        throw std::runtime_error("expected function body tokens");
    }

    if (functions[0].body_tokens.front().kind != token_kind::keyword_return) {
        throw std::runtime_error("unexpected first function body token");
    }
}

BASICPP_TEST(ParserRejectsFunctionWithoutEnd) {
    const std::string source =
        "module App\n"
        "function Main()\n"
        "return\n";

    auto tokens = lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = parser::parse_module(tokens.value());
    if (module) {
        throw std::runtime_error("parser should have failed");
    }

    if (module.error() != "expected 'end function' before end of input") {
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

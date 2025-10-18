#include <stdexcept>
#include <string>

#include <basicpp/testing/selftest.hpp>

#include "codegen/generator.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"

namespace {

std::string generate_cpp(std::string_view source) {
    auto tokens = basicpp::frontend::lexer::tokenize(source);
    if (!tokens) {
        throw std::runtime_error("lexer failed");
    }

    auto module = basicpp::frontend::parser::parse_module(tokens.value());
    if (!module) {
        throw std::runtime_error("parser failed");
    }

    auto generated = basicpp::codegen::generate_translation_unit(module.value());
    if (!generated) {
        throw std::runtime_error("code generation failed");
    }

    return generated.value();
}

} // namespace

BASICPP_TEST(CodegenEmitsConstStateCommandFunction) {
    constexpr std::string_view source =
        "module Demo\n"
        "import Basicpp.Command\n"
        "const Version = \"0.1.0\"\n"
        "state Machine = Idle\n"
        "on Start => Running\n"
        "on Stop => Idle\n"
        "command SayHello(name, context)\n"
        "return\n"
        "end command\n"
        "function Main(args) as Integer\n"
        "return\n"
        "end function\n";

    const auto cpp = generate_cpp(source);

    if (cpp.find("inline constexpr std::string_view Version{\"0.1.0\"};") == std::string::npos) {
        throw std::runtime_error("missing constant generation");
    }

    if (cpp.find("basicpp::state::state_machine<std::string, std::string>") == std::string::npos) {
        throw std::runtime_error("missing state machine generation");
    }

    if (cpp.find("core::result<void, std::string>::ok()") == std::string::npos) {
        throw std::runtime_error("missing command success stub");
    }

    if (cpp.find("return {};\n}") == std::string::npos) {
        throw std::runtime_error("missing function default return");
    }

    if (cpp.find("#include <basicpp/core/result.hpp>") == std::string::npos) {
        throw std::runtime_error("missing core result include");
    }

    if (cpp.find("#include <stdexcept>") != std::string::npos) {
        throw std::runtime_error("unexpected stdexcept include");
    }
}

BASICPP_TEST_MAIN()

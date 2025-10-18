#include "transpile.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#include "codegen/generator.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"

namespace basicpp::cli {

namespace {

struct transpile_options {
    std::filesystem::path input_path;
    std::optional<std::filesystem::path> output_path;
    bool show_tokens = false;
};

transpile_options parse_transpile_options(const std::vector<std::string>& params) {
    transpile_options options;

    for (std::size_t index = 0; index < params.size(); ++index) {
        const auto& param = params[index];

        if (param == "--tokens") {
            options.show_tokens = true;
            continue;
        }

        constexpr std::string_view out_prefix = "--out=";
        if (param.rfind(out_prefix, 0) == 0) {
            options.output_path = param.substr(out_prefix.size());
            continue;
        }

        if (param == "--out") {
            if (index + 1 >= params.size()) {
                throw std::runtime_error("--out requires a path argument");
            }
            options.output_path = params[++index];
            continue;
        }

        if (options.input_path.empty()) {
            options.input_path = param;
            continue;
        }

        throw std::runtime_error("unknown parameter: " + param);
    }

    if (options.input_path.empty()) {
        throw std::runtime_error("transpile requires an input file");
    }

    return options;
}

std::filesystem::path resolve_output_path(const std::filesystem::path& input,
                                          const std::optional<std::filesystem::path>& override_path) {
    if (override_path) {
        std::filesystem::path candidate = *override_path;
        if (!candidate.has_extension()) {
            std::filesystem::path filename = input.filename();
            filename.replace_extension(".cpp");
            candidate /= filename;
            return candidate;
        }

        if (candidate.extension() != ".cpp") {
            candidate.replace_extension(".cpp");
        }
        return candidate;
    }

    std::filesystem::path candidate = input;
    candidate.replace_extension(".cpp");
    return candidate;
}

int dump_tokens(const std::vector<basicpp::frontend::token>& tokens) {
    for (const auto& tok : tokens) {
        std::cout << tok.line << ':' << tok.column << '\t' << basicpp::frontend::to_string(tok.kind);
        if (!tok.lexeme.empty()) {
            std::cout << '\t' << tok.lexeme;
        }
        std::cout << '\n';
    }
    return 0;
}

} // namespace

int run_transpile(const std::vector<std::string>& params) {
    transpile_options options;
    try {
        options = parse_transpile_options(params);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    std::ifstream file(options.input_path, std::ios::binary);
    if (!file) {
        std::cerr << "failed to open " << options.input_path << '\n';
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto tokens_result = basicpp::frontend::lexer::tokenize(source);
    if (!tokens_result) {
        std::cerr << "lexer error: " << tokens_result.error() << '\n';
        return 1;
    }

    if (options.show_tokens) {
        return dump_tokens(tokens_result.value());
    }

    auto module = basicpp::frontend::parser::parse_module(tokens_result.value());
    if (!module) {
        std::cerr << "parser error: " << module.error() << '\n';
        return 1;
    }

    auto generated = basicpp::codegen::generate_translation_unit(module.value());
    if (!generated) {
        std::cerr << "codegen error: " << generated.error() << '\n';
        return 1;
    }

    const auto& cpp_source = generated.value();
    const auto output_path = resolve_output_path(options.input_path, options.output_path);

    if (output_path.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(output_path.parent_path(), ec);
        if (ec) {
            std::cerr << "failed to create output directory: " << ec.message() << '\n';
            return 1;
        }
    }

    std::ofstream output_file(output_path, std::ios::binary);
    if (!output_file) {
        std::cerr << "failed to write " << output_path.string() << '\n';
        return 1;
    }

    output_file << cpp_source;
    if (!cpp_source.empty() && cpp_source.back() != '\n') {
        output_file << '\n';
    }
    output_file.close();
    if (!output_file) {
        std::cerr << "failed to write " << output_path.string() << '\n';
        return 1;
    }

    std::cout << "Generated " << output_path.string() << '\n';
    return 0;
}

} // namespace basicpp::cli

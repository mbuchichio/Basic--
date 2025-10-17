#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "frontend/lexer.hpp"

namespace {

struct command_args {
    std::string subcommand;
    std::vector<std::string> parameters;
};

command_args parse_args(int argc, char** argv) {
    command_args result;
    if (argc > 1) {
        result.subcommand = argv[1];
        for (int i = 2; i < argc; ++i) {
            result.parameters.push_back(argv[i]);
        }
    }
    return result;
}

void print_usage() {
    std::cout << "Basic++ CLI (bppc)\n";
    std::cout << "Usage: bppc <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  transpile   Convert .bpp sources into C++ files\n";
    std::cout << "  build       Run full pipeline (transpile + compile)\n";
    std::cout << "  version     Display tool version\n";
}

int dump_tokens(const std::vector<basicpp::frontend::token>& tokens) {
    for (const auto& tok : tokens) {
        std::cout << tok.line << ':' << tok.column << "\t" << basicpp::frontend::to_string(tok.kind);
        if (!tok.lexeme.empty()) {
            std::cout << "\t" << tok.lexeme;
        }
        std::cout << '\n';
    }
    return 0;
}

int run_transpile(const std::vector<std::string>& params) {
    if (params.empty()) {
        std::cerr << "transpile requires an input file\n";
        return 1;
    }

    const std::string& input_path = params.front();
    std::ifstream file(input_path, std::ios::binary);
    if (!file) {
        std::cerr << "failed to open " << input_path << '\n';
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto tokens_result = basicpp::frontend::lexer::tokenize(source);
    if (!tokens_result) {
        std::cerr << "lexer error: " << tokens_result.error() << '\n';
        return 1;
    }

    return dump_tokens(tokens_result.value());
}

int run_build(const std::vector<std::string>& params) {
    std::cout << "[todo] build pipeline not yet implemented\n";
    return params.empty() ? 0 : 0;
}

int run_version() {
    std::cout << "bppc version 0.0.1 (runtime " << BASICPP_VERSION << ")\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    const auto args = parse_args(argc, argv);
    if (args.subcommand.empty()) {
        print_usage();
        return 1;
    }

    if (args.subcommand == "transpile") {
        return run_transpile(args.parameters);
    }

    if (args.subcommand == "build") {
        return run_build(args.parameters);
    }

    if (args.subcommand == "version") {
        return run_version();
    }

    std::cerr << "Unknown command: " << args.subcommand << "\n\n";
    print_usage();
    return 1;
}

#include <iostream>
#include <string>
#include <vector>

#include "transpile.hpp"

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
    std::cout << "\nOptions for 'transpile':\n";
    std::cout << "  --tokens           Dump lexer tokens after parsing\n";
    std::cout << "  --out <path>       Override output path (file or directory)\n";
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
        return basicpp::cli::run_transpile(args.parameters);
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

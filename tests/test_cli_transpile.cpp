#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <basicpp/testing/selftest.hpp>

#include "cli/transpile.hpp"

namespace {

std::filesystem::path make_temp_directory() {
    auto base = std::filesystem::current_path() / "cli_transpile_test";
    std::error_code ec;
    std::filesystem::remove_all(base, ec);
    std::filesystem::create_directories(base);
    return base;
}

} // namespace

BASICPP_TEST(CliTranspileWritesCppFile) {
    const auto temp_dir = make_temp_directory();
    const auto input_path = temp_dir / "Sample.bpp";
    const auto expected_output = temp_dir / "Sample.cpp";

    {
        std::ofstream input(input_path, std::ios::binary);
        if (!input) {
            throw std::runtime_error("failed to write input test file");
        }
        input << "module Demo\n"
              << "const Version = \"0.1\"\n";
    }

    if (std::filesystem::exists(expected_output)) {
        std::filesystem::remove(expected_output);
    }

    std::vector<std::string> params{input_path.string()};
    int exit_code = basicpp::cli::run_transpile(params);
    if (exit_code != 0) {
        throw std::runtime_error("run_transpile returned non-zero exit code");
    }

    if (!std::filesystem::exists(expected_output)) {
        throw std::runtime_error("expected output file was not created");
    }

    std::ifstream output(expected_output, std::ios::binary);
    if (!output) {
        throw std::runtime_error("failed to open generated cpp file");
    }
    std::string contents((std::istreambuf_iterator<char>(output)), std::istreambuf_iterator<char>());

    if (contents.find("basicpp_generated") == std::string::npos) {
        throw std::runtime_error("generated file did not contain expected namespace");
    }

    std::error_code ec;
    std::filesystem::remove_all(temp_dir, ec);
}

BASICPP_TEST_MAIN()

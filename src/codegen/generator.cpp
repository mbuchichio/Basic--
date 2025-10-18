#include "generator.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "frontend/token.hpp"

namespace basicpp::codegen {

namespace {

struct include_flags {
    bool string_header = false;
    bool string_view_header = false;
    bool cstdint_header = false;
    bool stdexcept_header = false;
    bool core_result_header = false;
    bool state_machine_header = false;
};

std::string sanitize_identifier(const std::string& name) {
    std::string result;
    result.reserve(name.size() + 1);

    auto is_valid = [](char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
    };

    if (name.empty()) {
        return "_";
    }

    if (!std::isalpha(static_cast<unsigned char>(name.front())) && name.front() != '_') {
        result.push_back('_');
    }

    for (char ch : name) {
        result.push_back(is_valid(ch) ? ch : '_');
    }

    return result;
}

std::string escape_string(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size() + 8);
    for (char ch : value) {
        switch (ch) {
        case '\\': escaped += "\\\\"; break;
        case '"': escaped += "\\\""; break;
        case '\n': escaped += "\\n"; break;
        case '\r': escaped += "\\r"; break;
        case '\t': escaped += "\\t"; break;
        default:
            if (std::isprint(static_cast<unsigned char>(ch))) {
                escaped.push_back(ch);
            } else {
                constexpr char hex_digits[] = "0123456789ABCDEF";
                escaped += "\\x";
                escaped.push_back(hex_digits[(static_cast<unsigned char>(ch) >> 4) & 0xF]);
                escaped.push_back(hex_digits[static_cast<unsigned char>(ch) & 0xF]);
            }
            break;
        }
    }
    return escaped;
}

std::string render_constant_declaration(const frontend::ast::const_decl& decl, include_flags& flags) {
    const auto variable_name = sanitize_identifier(decl.name);
    const auto& literal = decl.value;

    switch (literal.kind) {
    case frontend::token_kind::string_literal: {
        flags.string_view_header = true;
        return "inline constexpr std::string_view " + variable_name + "{\"" + escape_string(literal.lexeme) + "\"};";
    }
    case frontend::token_kind::integer_literal:
    case frontend::token_kind::float_literal:
    case frontend::token_kind::keyword_true:
    case frontend::token_kind::keyword_false:
        return "inline constexpr auto " + variable_name + " = " + literal.lexeme + ";";
    default:
        throw std::logic_error("unsupported literal kind in constant generation");
    }
}

std::string convert_type_name(const std::string& type_name, include_flags& flags) {
    if (type_name == "Integer") {
        flags.cstdint_header = true;
        return "std::int64_t";
    }
    if (type_name == "Float") {
        return "double";
    }
    if (type_name == "String") {
        flags.string_header = true;
        return "std::string";
    }
    if (type_name == "Boolean") {
        return "bool";
    }

    std::string converted = type_name;
    std::replace(converted.begin(), converted.end(), '.', ':');

    // Replace single ':' occurrences with '::'
    for (std::size_t i = 0; i < converted.size(); ++i) {
        if (converted[i] == ':') {
            converted.insert(i, 1, ':');
            ++i;
        }
    }

    return converted;
}

std::string render_state_factory(const frontend::ast::state_decl& state, include_flags& flags) {
    flags.string_header = true;
    flags.state_machine_header = true;

    const auto function_name = "make_" + sanitize_identifier(state.name) + "_state";
    std::ostringstream out;
    out << "inline basicpp::state::state_machine<std::string, std::string> " << function_name << "()\n";
    out << "{\n";
    out << "    basicpp::state::state_machine<std::string, std::string> machine{\"" << escape_string(state.initial_state)
        << "\"};\n";

    std::string current_state = state.initial_state;
    for (const auto& transition : state.transitions) {
        out << "    machine.add_transition(\"" << escape_string(current_state) << "\", \""
            << escape_string(transition.event) << "\", \"" << escape_string(transition.target_state) << "\");\n";
        current_state = transition.target_state;
    }

    out << "    return machine;\n";
    out << "}\n";
    return out.str();
}

std::string render_command(const frontend::ast::command_decl& command, include_flags& flags) {
    flags.string_header = true;
    flags.core_result_header = true;

    const auto function_name = sanitize_identifier(command.name);
    std::ostringstream out;
    out << "inline basicpp::core::result<void, std::string> " << function_name << "(";

    for (std::size_t i = 0; i < command.parameters.size(); ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << "const std::string& " << sanitize_identifier(command.parameters[i]);
    }

    out << ")\n";
    out << "{\n";

    for (const auto& param : command.parameters) {
        out << "    (void)" << sanitize_identifier(param) << ";\n";
    }

    out << "    // TODO: Translate Basic++ command body into C++\n";
    out << "    return basicpp::core::result<void, std::string>::ok();\n";
    out << "}\n";

    return out.str();
}

std::string render_function(const frontend::ast::function_decl& fn, include_flags& flags) {
    flags.string_header = true; // parameters use std::string

    const auto function_name = sanitize_identifier(fn.name);
    const bool has_return_type = fn.return_type.has_value();
    const std::string return_type = has_return_type ? convert_type_name(fn.return_type.value(), flags) : "void";

    std::ostringstream out;
    out << "inline " << return_type << ' ' << function_name << '(';

    for (std::size_t i = 0; i < fn.parameters.size(); ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << "const std::string& " << sanitize_identifier(fn.parameters[i]);
    }

    out << ")\n";
    out << "{\n";

    for (const auto& param : fn.parameters) {
        out << "    (void)" << sanitize_identifier(param) << ";\n";
    }

    out << "    // TODO: Translate Basic++ function body into C++\n";
    if (has_return_type) {
        out << "    return {};\n";
    } else {
        out << "    return;\n";
    }

    out << "}\n";

    return out.str();
}

} // namespace

core::result<std::string, std::string> generate_translation_unit(const frontend::ast::module_decl& module) {
    include_flags flags{};

    std::ostringstream constants_section;
    for (const auto& constant : module.constants) {
        constants_section << render_constant_declaration(constant, flags) << "\n";
    }

    if (!module.constants.empty()) {
        constants_section << '\n';
    }

    std::ostringstream states_section;
    for (const auto& state : module.states) {
        states_section << render_state_factory(state, flags) << '\n';
    }

    std::ostringstream commands_section;
    for (const auto& command : module.commands) {
        commands_section << render_command(command, flags) << '\n';
    }

    std::ostringstream functions_section;
    for (const auto& fn : module.functions) {
        functions_section << render_function(fn, flags) << '\n';
    }

    std::ostringstream out;
    out << "// Generated by bppc 0.0.1\n";
    out << "// Module: " << module.name << "\n\n";

    if (flags.string_header) {
        out << "#include <string>\n";
    }
    if (flags.string_view_header) {
        out << "#include <string_view>\n";
    }
    if (flags.cstdint_header) {
        out << "#include <cstdint>\n";
    }
    if (flags.stdexcept_header) {
        out << "#include <stdexcept>\n";
    }
    if (flags.core_result_header) {
        out << "#include <basicpp/core/result.hpp>\n";
    }
    if (flags.state_machine_header) {
        out << "#include <basicpp/state/state_machine.hpp>\n";
    }

    if (!module.imports.empty()) {
        out << '\n';
        out << "// Basic++ imports\n";
        for (const auto& import : module.imports) {
            out << "//   - " << import.path << "\n";
        }
    }

    out << '\n';

    const auto module_namespace = sanitize_identifier(module.name);
    out << "namespace basicpp_generated {\n";
    out << "namespace " << module_namespace << " {\n\n";

    out << constants_section.str();
    out << states_section.str();
    out << commands_section.str();
    out << functions_section.str();

    out << "} // namespace " << module_namespace << "\n";
    out << "} // namespace basicpp_generated\n";

    return core::result<std::string, std::string>::ok(out.str());
}

} // namespace basicpp::codegen

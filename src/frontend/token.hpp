#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace basicpp::frontend {

enum class token_kind {
    end_of_file,
    identifier,
    integer_literal,
    float_literal,
    string_literal,
    keyword_module,
    keyword_import,
    keyword_const,
    keyword_command,
    keyword_state,
    keyword_on,
    keyword_function,
    keyword_as,
    keyword_end,
    keyword_if,
    keyword_then,
    keyword_else,
    keyword_elseif,
    keyword_for,
    keyword_in,
    keyword_while,
    keyword_return,
    keyword_result,
    keyword_error,
    keyword_ok,
    keyword_let,
    l_paren,
    r_paren,
    l_brace,
    r_brace,
    l_bracket,
    r_bracket,
    comma,
    dot,
    colon,
    semicolon,
    assignment,
    arrow,
    plus,
    minus,
    star,
    slash,
    caret,
    ampersand,
    equals,
    not_equals,
    less,
    less_equal,
    greater,
    greater_equal,
};

struct token {
    token_kind kind{};
    std::string lexeme{};
    std::size_t line = 1;
    std::size_t column = 1;
};

inline std::string_view to_string(token_kind kind) {
    switch (kind) {
    case token_kind::end_of_file: return "eof";
    case token_kind::identifier: return "identifier";
    case token_kind::integer_literal: return "integer";
    case token_kind::float_literal: return "float";
    case token_kind::string_literal: return "string";
    case token_kind::keyword_module: return "module";
    case token_kind::keyword_import: return "import";
    case token_kind::keyword_const: return "const";
    case token_kind::keyword_command: return "command";
    case token_kind::keyword_state: return "state";
    case token_kind::keyword_on: return "on";
    case token_kind::keyword_function: return "function";
    case token_kind::keyword_as: return "as";
    case token_kind::keyword_end: return "end";
    case token_kind::keyword_if: return "if";
    case token_kind::keyword_then: return "then";
    case token_kind::keyword_else: return "else";
    case token_kind::keyword_elseif: return "elseif";
    case token_kind::keyword_for: return "for";
    case token_kind::keyword_in: return "in";
    case token_kind::keyword_while: return "while";
    case token_kind::keyword_return: return "return";
    case token_kind::keyword_result: return "result";
    case token_kind::keyword_error: return "error";
    case token_kind::keyword_ok: return "ok";
    case token_kind::keyword_let: return "let";
    case token_kind::l_paren: return "(";
    case token_kind::r_paren: return ")";
    case token_kind::l_brace: return "{";
    case token_kind::r_brace: return "}";
    case token_kind::l_bracket: return "[";
    case token_kind::r_bracket: return "]";
    case token_kind::comma: return ",";
    case token_kind::dot: return ".";
    case token_kind::colon: return ":";
    case token_kind::semicolon: return ";";
    case token_kind::assignment: return "=";
    case token_kind::arrow: return "=>";
    case token_kind::plus: return "+";
    case token_kind::minus: return "-";
    case token_kind::star: return "*";
    case token_kind::slash: return "/";
    case token_kind::caret: return "^";
    case token_kind::ampersand: return "&";
    case token_kind::equals: return "==";
    case token_kind::not_equals: return "<>";
    case token_kind::less: return "<";
    case token_kind::less_equal: return "<=";
    case token_kind::greater: return ">";
    case token_kind::greater_equal: return ">=";
    }
    return "unknown";
}

} // namespace basicpp::frontend

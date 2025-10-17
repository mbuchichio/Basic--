#include "lexer.hpp"

#include <cctype>
#include <optional>
#include <utility>

namespace basicpp::frontend {

namespace {

struct cursor {
    std::string_view source;
    std::size_t index = 0;
    std::size_t line = 1;
    std::size_t column = 1;

    bool at_end() const noexcept {
        return index >= source.size();
    }

    char peek() const noexcept {
        return at_end() ? '\0' : source[index];
    }

    char peek_next() const noexcept {
        return (index + 1 < source.size()) ? source[index + 1] : '\0';
    }

    char advance() {
        char c = peek();
        if (c == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
        ++index;
        return c;
    }

    bool match(char expected) {
        if (peek() != expected) {
            return false;
        }
        advance();
        return true;
    }
};

bool is_identifier_start(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool is_identifier_part(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::optional<token_kind> keyword_lookup(std::string_view lexeme) {
    using pair = std::pair<std::string_view, token_kind>;
    static constexpr pair keywords[] = {
        {"module", token_kind::keyword_module},
        {"import", token_kind::keyword_import},
        {"const", token_kind::keyword_const},
        {"command", token_kind::keyword_command},
        {"state", token_kind::keyword_state},
        {"on", token_kind::keyword_on},
        {"function", token_kind::keyword_function},
        {"end", token_kind::keyword_end},
        {"if", token_kind::keyword_if},
        {"then", token_kind::keyword_then},
        {"else", token_kind::keyword_else},
        {"elseif", token_kind::keyword_elseif},
        {"for", token_kind::keyword_for},
        {"in", token_kind::keyword_in},
        {"while", token_kind::keyword_while},
        {"return", token_kind::keyword_return},
        {"result", token_kind::keyword_result},
        {"error", token_kind::keyword_error},
        {"ok", token_kind::keyword_ok},
        {"let", token_kind::keyword_let},
    };

    for (const auto& [word, kind] : keywords) {
        if (lexeme == word) {
            return kind;
        }
    }
    return std::nullopt;
}

} // namespace

core::result<std::vector<token>, std::string> lexer::tokenize(std::string_view source) {
    std::vector<token> tokens;
    cursor cur{source};

    auto make_token = [&](token_kind kind, std::string lexeme, std::size_t line, std::size_t column) {
        return token{kind, std::move(lexeme), line, column};
    };

    auto add_simple_token = [&](token_kind kind, std::size_t start_index, std::size_t start_line, std::size_t start_column) {
        std::string lexeme(source.substr(start_index, cur.index - start_index));
        tokens.push_back(make_token(kind, std::move(lexeme), start_line, start_column));
    };

    while (!cur.at_end()) {
        const std::size_t token_start_index = cur.index;
        const std::size_t token_start_line = cur.line;
        const std::size_t token_start_column = cur.column;
        char c = cur.advance();

        switch (c) {
        case ' ': case '\r': case '\t':
            break;
        case '\n':
            break;
        case '(':
            add_simple_token(token_kind::l_paren, token_start_index, token_start_line, token_start_column);
            break;
        case ')':
            add_simple_token(token_kind::r_paren, token_start_index, token_start_line, token_start_column);
            break;
        case '{':
            add_simple_token(token_kind::l_brace, token_start_index, token_start_line, token_start_column);
            break;
        case '}':
            add_simple_token(token_kind::r_brace, token_start_index, token_start_line, token_start_column);
            break;
        case '[':
            add_simple_token(token_kind::l_bracket, token_start_index, token_start_line, token_start_column);
            break;
        case ']':
            add_simple_token(token_kind::r_bracket, token_start_index, token_start_line, token_start_column);
            break;
        case ',':
            add_simple_token(token_kind::comma, token_start_index, token_start_line, token_start_column);
            break;
        case '.':
            add_simple_token(token_kind::dot, token_start_index, token_start_line, token_start_column);
            break;
        case ':':
            add_simple_token(token_kind::colon, token_start_index, token_start_line, token_start_column);
            break;
        case ';':
            add_simple_token(token_kind::semicolon, token_start_index, token_start_line, token_start_column);
            break;
        case '+':
            add_simple_token(token_kind::plus, token_start_index, token_start_line, token_start_column);
            break;
        case '-':
            if (cur.match('>')) {
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                tokens.push_back(make_token(token_kind::arrow, std::move(lexeme), token_start_line, token_start_column));
            } else {
                add_simple_token(token_kind::minus, token_start_index, token_start_line, token_start_column);
            }
            break;
        case '*':
            add_simple_token(token_kind::star, token_start_index, token_start_line, token_start_column);
            break;
        case '/':
            if (cur.match('/')) {
                // Skip comment until end of line.
                while (!cur.at_end() && cur.peek() != '\n') {
                    cur.advance();
                }
            } else {
                add_simple_token(token_kind::slash, token_start_index, token_start_line, token_start_column);
            }
            break;
        case '^':
            add_simple_token(token_kind::caret, token_start_index, token_start_line, token_start_column);
            break;
        case '&':
            add_simple_token(token_kind::ampersand, token_start_index, token_start_line, token_start_column);
            break;
        case '=':
            if (cur.match('=')) {
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                tokens.push_back(make_token(token_kind::equals, std::move(lexeme), token_start_line, token_start_column));
            } else {
                add_simple_token(token_kind::assignment, token_start_index, token_start_line, token_start_column);
            }
            break;
        case '<':
            if (cur.match('=')) {
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                tokens.push_back(make_token(token_kind::less_equal, std::move(lexeme), token_start_line, token_start_column));
            } else if (cur.match('>')) {
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                tokens.push_back(make_token(token_kind::not_equals, std::move(lexeme), token_start_line, token_start_column));
            } else {
                add_simple_token(token_kind::less, token_start_index, token_start_line, token_start_column);
            }
            break;
        case '>':
            if (cur.match('=')) {
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                tokens.push_back(make_token(token_kind::greater_equal, std::move(lexeme), token_start_line, token_start_column));
            } else {
                add_simple_token(token_kind::greater, token_start_index, token_start_line, token_start_column);
            }
            break;
        case '"': {
            while (!cur.at_end() && cur.peek() != '"') {
                if (cur.peek() == '\n') {
                    // allow multiline strings but update counters via advance
                }
                cur.advance();
            }
            if (cur.at_end()) {
                return core::result<std::vector<token>, std::string>::err("unterminated string literal");
            }
            cur.advance(); // closing quote
            std::string lexeme(source.substr(token_start_index + 1, cur.index - token_start_index - 2));
            tokens.push_back(make_token(token_kind::string_literal, std::move(lexeme), token_start_line, token_start_column));
            break;
        }
        default:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                bool is_float = false;
                while (std::isdigit(static_cast<unsigned char>(cur.peek()))) {
                    cur.advance();
                }
                if (cur.peek() == '.' && std::isdigit(static_cast<unsigned char>(cur.peek_next()))) {
                    is_float = true;
                    cur.advance(); // consume '.'
                    while (std::isdigit(static_cast<unsigned char>(cur.peek()))) {
                        cur.advance();
                    }
                }
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                tokens.push_back(make_token(is_float ? token_kind::float_literal : token_kind::integer_literal,
                                            std::move(lexeme), token_start_line, token_start_column));
            } else if (is_identifier_start(c)) {
                while (is_identifier_part(cur.peek())) {
                    cur.advance();
                }
                std::string lexeme(source.substr(token_start_index, cur.index - token_start_index));
                if (auto keyword = keyword_lookup(lexeme)) {
                    tokens.push_back(make_token(*keyword, std::move(lexeme), token_start_line, token_start_column));
                } else {
                    tokens.push_back(make_token(token_kind::identifier, std::move(lexeme), token_start_line, token_start_column));
                }
            } else {
                return core::result<std::vector<token>, std::string>::err("unexpected character");
            }
            break;
        }
    }

    tokens.push_back(token{token_kind::end_of_file, "", cur.line, cur.column});
    return core::result<std::vector<token>, std::string>::ok(std::move(tokens));
}

} // namespace basicpp::frontend

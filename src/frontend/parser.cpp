#include "parser.hpp"

#include <cstddef>
#include <string>
#include <utility>

namespace basicpp::frontend {

namespace {

class parser_impl {
public:
    explicit parser_impl(const std::vector<token>& tokens)
        : tokens_(tokens) {
    }

    core::result<ast::module_decl, std::string> parse_module() {
        ast::module_decl module;

        if (match(token_kind::end_of_file)) {
            return core::result<ast::module_decl, std::string>::err("empty input");
        }

        if (!match(token_kind::keyword_module)) {
            return core::result<ast::module_decl, std::string>::err("expected 'module' keyword");
        }

        auto name_token = consume(token_kind::identifier, "expected module name");
        if (!name_token) {
            return core::result<ast::module_decl, std::string>::err(name_token.error());
        }

        module.name = name_token.value().lexeme;

        while (peek(token_kind::keyword_import)) {
            advance();
            auto import = parse_import_path();
            if (!import) {
                return core::result<ast::module_decl, std::string>::err(import.error());
            }
            module.imports.push_back(std::move(import.value()));
        }

        while (!is_at_end() && !peek(token_kind::end_of_file)) {
            if (peek(token_kind::keyword_const)) {
                advance();
                auto constant = parse_const_decl();
                if (!constant) {
                    return core::result<ast::module_decl, std::string>::err(constant.error());
                }
                module.constants.push_back(std::move(constant.value()));
                continue;
            }

            return core::result<ast::module_decl, std::string>::err(unexpected_token_message());
        }

        // Future work: parse additional declarations following constants.

        return core::result<ast::module_decl, std::string>::ok(std::move(module));
    }

private:
    const token& current() const {
        return tokens_[index_];
    }

    bool match(token_kind kind) {
        if (peek(kind)) {
            advance();
            return true;
        }
        return false;
    }

    bool peek(token_kind kind) const {
        return !is_at_end() && current().kind == kind;
    }

    void advance() {
        if (!is_at_end()) {
            ++index_;
        }
    }

    bool is_at_end() const {
        return index_ >= tokens_.size();
    }

    core::result<token, std::string> consume(token_kind kind, const char* message) {
        if (peek(kind)) {
            token tok = current();
            advance();
            return core::result<token, std::string>::ok(std::move(tok));
        }
        if (is_at_end()) {
            return core::result<token, std::string>::err(std::string(message));
        }
        std::string error = message;
        error += ": found '";
        error += current().lexeme;
        error += "'";
        return core::result<token, std::string>::err(std::move(error));
    }

    core::result<ast::import_decl, std::string> parse_import_path() {
        auto first = consume(token_kind::identifier, "expected identifier after 'import'");
        if (!first) {
            return core::result<ast::import_decl, std::string>::err("expected identifier after 'import'");
        }

        std::string path = first.value().lexeme;

        while (match(token_kind::dot)) {
            auto segment = consume(token_kind::identifier, "expected identifier after '.' in import path");
            if (!segment) {
                return core::result<ast::import_decl, std::string>::err("expected identifier after '.' in import path");
            }
            path.push_back('.');
            path += segment.value().lexeme;
        }

        ast::import_decl import{std::move(path)};
        return core::result<ast::import_decl, std::string>::ok(std::move(import));
    }

    core::result<ast::const_decl, std::string> parse_const_decl() {
        auto name_token = consume(token_kind::identifier, "expected constant name");
        if (!name_token) {
            return core::result<ast::const_decl, std::string>::err(name_token.error());
        }

        auto equals_token = consume(token_kind::assignment, "expected '=' after constant name");
        if (!equals_token) {
            return core::result<ast::const_decl, std::string>::err(equals_token.error());
        }

        auto value = parse_literal();
        if (!value) {
            return core::result<ast::const_decl, std::string>::err(value.error());
        }

        ast::const_decl decl{};
        decl.name = name_token.value().lexeme;
        decl.value = std::move(value.value());
        return core::result<ast::const_decl, std::string>::ok(std::move(decl));
    }

    core::result<ast::literal, std::string> parse_literal() {
        if (peek(token_kind::string_literal) || peek(token_kind::integer_literal) || peek(token_kind::float_literal)) {
            token tok = current();
            advance();
            return core::result<ast::literal, std::string>::ok(ast::literal{tok.kind, tok.lexeme});
        }

        if (is_at_end()) {
            return core::result<ast::literal, std::string>::err("expected literal after '='");
        }

        std::string message = "expected literal after '=': found '";
        message += current().lexeme;
        message += "'";
        return core::result<ast::literal, std::string>::err(std::move(message));
    }

    std::string unexpected_token_message() const {
        if (is_at_end()) {
            return "unexpected end of input";
        }

        std::string message = "unexpected token '";
        message += current().lexeme;
        message += "'";
        return message;
    }

    const std::vector<token>& tokens_;
    std::size_t index_ = 0;
};

} // namespace

core::result<ast::module_decl, std::string> parser::parse_module(const std::vector<token>& tokens) {
    parser_impl impl(tokens);
    return impl.parse_module();
}

} // namespace basicpp::frontend

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

        // Future work: parse imports and declarations. For now we accept the remainder.

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

    const std::vector<token>& tokens_;
    std::size_t index_ = 0;
};

} // namespace

core::result<ast::module_decl, std::string> parser::parse_module(const std::vector<token>& tokens) {
    parser_impl impl(tokens);
    return impl.parse_module();
}

} // namespace basicpp::frontend

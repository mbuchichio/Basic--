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

            if (peek(token_kind::keyword_state)) {
                advance();
                auto state = parse_state_decl();
                if (!state) {
                    return core::result<ast::module_decl, std::string>::err(state.error());
                }
                module.states.push_back(std::move(state.value()));
                continue;
            }

            if (peek(token_kind::keyword_command)) {
                advance();
                auto command = parse_command_decl();
                if (!command) {
                    return core::result<ast::module_decl, std::string>::err(command.error());
                }
                module.commands.push_back(std::move(command.value()));
                continue;
            }

            if (peek(token_kind::keyword_function)) {
                advance();
                auto function = parse_function_decl();
                if (!function) {
                    return core::result<ast::module_decl, std::string>::err(function.error());
                }
                module.functions.push_back(std::move(function.value()));
                continue;
            }

            return core::result<ast::module_decl, std::string>::err(unexpected_token_message());
        }

        // Future work: parse additional declarations.

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

    bool peek_next(token_kind kind) const {
        if (index_ + 1 >= tokens_.size()) {
            return false;
        }
        return tokens_[index_ + 1].kind == kind;
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

    core::result<ast::command_decl, std::string> parse_command_decl() {
        auto name_token = consume(token_kind::identifier, "expected command name");
        if (!name_token) {
            return core::result<ast::command_decl, std::string>::err(name_token.error());
        }

        auto open_paren = consume(token_kind::l_paren, "expected '(' after command name");
        if (!open_paren) {
            return core::result<ast::command_decl, std::string>::err(open_paren.error());
        }

        auto parameters = parse_parameter_list();
        if (!parameters) {
            return core::result<ast::command_decl, std::string>::err(parameters.error());
        }

        auto close_paren = consume(token_kind::r_paren, "expected ')' after command parameters");
        if (!close_paren) {
            return core::result<ast::command_decl, std::string>::err(close_paren.error());
        }

    auto body = parse_block_body(token_kind::keyword_command, "command", "expected 'command' after 'end'");
        if (!body) {
            return core::result<ast::command_decl, std::string>::err(body.error());
        }

        ast::command_decl decl{};
        decl.name = name_token.value().lexeme;
        decl.parameters = std::move(parameters.value());
        decl.body_tokens = std::move(body.value());
        return core::result<ast::command_decl, std::string>::ok(std::move(decl));
    }

    core::result<ast::function_decl, std::string> parse_function_decl() {
        auto name_token = consume(token_kind::identifier, "expected function name");
        if (!name_token) {
            return core::result<ast::function_decl, std::string>::err(name_token.error());
        }

        auto open_paren = consume(token_kind::l_paren, "expected '(' after function name");
        if (!open_paren) {
            return core::result<ast::function_decl, std::string>::err(open_paren.error());
        }

        auto parameters = parse_parameter_list();
        if (!parameters) {
            return core::result<ast::function_decl, std::string>::err(parameters.error());
        }

        auto close_paren = consume(token_kind::r_paren, "expected ')' after function parameters");
        if (!close_paren) {
            return core::result<ast::function_decl, std::string>::err(close_paren.error());
        }

        std::optional<std::string> return_type;
        if (match(token_kind::keyword_as)) {
            auto type = parse_type_spec();
            if (!type) {
                return core::result<ast::function_decl, std::string>::err(type.error());
            }
            return_type = std::move(type.value());
        }

    auto body = parse_block_body(token_kind::keyword_function, "function", "expected 'function' after 'end'");
        if (!body) {
            return core::result<ast::function_decl, std::string>::err(body.error());
        }

        ast::function_decl decl{};
        decl.name = name_token.value().lexeme;
        decl.parameters = std::move(parameters.value());
        decl.return_type = std::move(return_type);
        decl.body_tokens = std::move(body.value());
        return core::result<ast::function_decl, std::string>::ok(std::move(decl));
    }

    core::result<ast::state_decl, std::string> parse_state_decl() {
        auto name_token = consume(token_kind::identifier, "expected state name");
        if (!name_token) {
            return core::result<ast::state_decl, std::string>::err(name_token.error());
        }

        auto equals_token = consume(token_kind::assignment, "expected '=' after state name");
        if (!equals_token) {
            return core::result<ast::state_decl, std::string>::err(equals_token.error());
        }

        auto initial_token = consume(token_kind::identifier, "expected initial state identifier");
        if (!initial_token) {
            return core::result<ast::state_decl, std::string>::err(initial_token.error());
        }

        if (!peek(token_kind::keyword_on)) {
            return core::result<ast::state_decl, std::string>::err("state requires at least one 'on' transition");
        }

        ast::state_decl decl{};
        decl.name = name_token.value().lexeme;
        decl.initial_state = initial_token.value().lexeme;

        while (peek(token_kind::keyword_on)) {
            advance(); // consume 'on'
            auto transition = parse_state_transition();
            if (!transition) {
                return core::result<ast::state_decl, std::string>::err(transition.error());
            }
            decl.transitions.push_back(std::move(transition.value()));
        }

        return core::result<ast::state_decl, std::string>::ok(std::move(decl));
    }

    core::result<ast::state_transition, std::string> parse_state_transition() {
        auto event_token = consume(token_kind::identifier, "expected event name after 'on'");
        if (!event_token) {
            return core::result<ast::state_transition, std::string>::err(event_token.error());
        }

        auto arrow_token = consume(token_kind::arrow, "expected '=>' after event name");
        if (!arrow_token) {
            return core::result<ast::state_transition, std::string>::err(arrow_token.error());
        }

        auto target_token = consume(token_kind::identifier, "expected target state after '=>'");
        if (!target_token) {
            return core::result<ast::state_transition, std::string>::err(target_token.error());
        }

        ast::state_transition transition{};
        transition.event = event_token.value().lexeme;
        transition.target_state = target_token.value().lexeme;
        return core::result<ast::state_transition, std::string>::ok(std::move(transition));
    }

    core::result<std::vector<std::string>, std::string> parse_parameter_list() {
        std::vector<std::string> parameters;
        if (peek(token_kind::r_paren)) {
            return core::result<std::vector<std::string>, std::string>::ok(std::move(parameters));
        }

        while (true) {
            auto param = consume(token_kind::identifier, "expected parameter name");
            if (!param) {
                return core::result<std::vector<std::string>, std::string>::err(param.error());
            }
            parameters.push_back(param.value().lexeme);

            if (!match(token_kind::comma)) {
                break;
            }
        }

        return core::result<std::vector<std::string>, std::string>::ok(std::move(parameters));
    }

    core::result<std::string, std::string> parse_type_spec() {
        auto first = consume(token_kind::identifier, "expected type name after 'as'");
        if (!first) {
            return core::result<std::string, std::string>::err(first.error());
        }

        std::string name = first.value().lexeme;

        while (match(token_kind::dot)) {
            auto segment = consume(token_kind::identifier, "expected identifier after '.' in type name");
            if (!segment) {
                return core::result<std::string, std::string>::err(segment.error());
            }
            name.push_back('.');
            name += segment.value().lexeme;
        }

        return core::result<std::string, std::string>::ok(std::move(name));
    }

    core::result<std::vector<token>, std::string> parse_block_body(token_kind closing_keyword, const char* block_name,
                                                                  const char* closing_keyword_message) {
        std::vector<token> body;

        while (!is_at_end()) {
            if (peek(token_kind::keyword_end) && peek_next(closing_keyword)) {
                break;
            }
            body.push_back(current());
            advance();
        }

        if (is_at_end()) {
            std::string message = "expected 'end ";
            message += block_name;
            message += "' before end of input";
            return core::result<std::vector<token>, std::string>::err(std::move(message));
        }

        auto end_token = consume(token_kind::keyword_end, "expected 'end' to close block");
        if (!end_token) {
            return core::result<std::vector<token>, std::string>::err(end_token.error());
        }

        auto keyword_token = consume(closing_keyword, closing_keyword_message);
        if (!keyword_token) {
            return core::result<std::vector<token>, std::string>::err(keyword_token.error());
        }

        return core::result<std::vector<token>, std::string>::ok(std::move(body));
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

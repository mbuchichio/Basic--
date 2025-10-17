#pragma once

#include <optional>
#include <stdexcept>
#include <utility>

namespace basicpp::core {

// Lightweight success/error carrier similar to std::expected
// Provides basic accessors and value_or helpers; throws std::logic_error on misuse.
template <typename T, typename E>
class result {
public:
    using value_type = T;
    using error_type = E;

    static result ok(T value) {
        return result(ok_tag{}, std::move(value));
    }

    static result err(E error) {
        return result(err_tag{}, std::move(error));
    }

    bool has_value() const noexcept {
        return value_.has_value();
    }

    explicit operator bool() const noexcept {
        return has_value();
    }

    const T& value() const& {
        ensure_value();
        return *value_;
    }

    T& value() & {
        ensure_value();
        return *value_;
    }

    T&& value() && {
        ensure_value();
        return std::move(*value_);
    }

    const E& error() const& {
        ensure_error();
        return *error_;
    }

    E& error() & {
        ensure_error();
        return *error_;
    }

    E&& error() && {
        ensure_error();
        return std::move(*error_);
    }

    template <typename U>
    T value_or(U&& fallback) const& {
        return value_.has_value() ? *value_ : static_cast<T>(std::forward<U>(fallback));
    }

    template <typename U>
    T value_or(U&& fallback) && {
        return value_.has_value() ? std::move(*value_) : static_cast<T>(std::forward<U>(fallback));
    }

private:
    struct ok_tag {};
    struct err_tag {};

    result(ok_tag, T value)
        : value_(std::move(value)) {
    }

    result(err_tag, E error)
        : error_(std::move(error)) {
    }

    void ensure_value() const {
        if (!value_) {
            throw std::logic_error("basicpp::core::result accessed without value");
        }
    }

    void ensure_error() const {
        if (!error_) {
            throw std::logic_error("basicpp::core::result accessed without error");
        }
    }

    std::optional<T> value_;
    std::optional<E> error_;
};

// Partial specialization for void success type.
template <typename E>
class result<void, E> {
public:
    using value_type = void;
    using error_type = E;

    static result ok() {
        return result(ok_tag{});
    }

    static result err(E error) {
        return result(err_tag{}, std::move(error));
    }

    bool has_value() const noexcept {
        return has_value_;
    }

    explicit operator bool() const noexcept {
        return has_value();
    }

    void value() const {
        if (!has_value_) {
            throw std::logic_error("basicpp::core::result<void, E> accessed without value");
        }
    }

    const E& error() const& {
        ensure_error();
        return *error_;
    }

    E& error() & {
        ensure_error();
        return *error_;
    }

    E&& error() && {
        ensure_error();
        return std::move(*error_);
    }

private:
    struct ok_tag {};
    struct err_tag {};

    explicit result(ok_tag)
        : has_value_(true) {
    }

    result(err_tag, E error)
        : has_value_(false), error_(std::move(error)) {
    }

    void ensure_error() const {
        if (!error_) {
            throw std::logic_error("basicpp::core::result<void, E> accessed without error");
        }
    }

    bool has_value_ = false;
    std::optional<E> error_;
};

} // namespace basicpp::core

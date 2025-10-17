#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <utility>

namespace basicpp::history {

// Coalesces a stream of values inside a fixed time window.
// Useful to collapse frequent updates (e.g., edits) before pushing to history stacks.
template <typename TValue, typename TClock = std::chrono::steady_clock>
class coalescer {
public:
    using value_type = TValue;
    using clock_type = TClock;
    using time_point = typename clock_type::time_point;
    using duration = typename clock_type::duration;
    using combine_fn = std::function<value_type(const value_type&, const value_type&)>;

    coalescer(duration window, combine_fn combine)
        : window_(window), combine_(std::move(combine)) {
    }

    bool has_pending() const noexcept {
        return pending_.has_value();
    }

    void reset() {
        pending_.reset();
        first_timestamp_.reset();
    }

    void push(const value_type& value, time_point now = clock_type::now()) {
        push_impl(value, now);
    }

    void push(value_type&& value, time_point now = clock_type::now()) {
        push_impl(std::move(value), now);
    }

    std::optional<value_type> consume(time_point now = clock_type::now()) {
        if (!pending_) {
            return std::nullopt;
        }

        if (!first_timestamp_) {
            first_timestamp_ = now;
            return std::nullopt;
        }

        if (now - *first_timestamp_ < window_) {
            return std::nullopt;
        }

        auto result = std::move(pending_);
        reset();
        return result;
    }

private:
    template <typename U>
    void push_impl(U&& value, time_point now) {
        if (!pending_) {
            pending_ = std::forward<U>(value);
            first_timestamp_ = now;
            return;
        }

    pending_ = combine_(*pending_, std::forward<U>(value));
        // Keep the original start time to honour the window from the first update.
    }

    duration window_;
    combine_fn combine_;
    std::optional<value_type> pending_;
    std::optional<time_point> first_timestamp_;
};

} // namespace basicpp::history

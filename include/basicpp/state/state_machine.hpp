#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <basicpp/core/result.hpp>

namespace basicpp::state {

namespace detail {
    template <typename TStateId, typename TEvent>
    struct transition_key {
        TStateId from;
        TEvent event;
    };

    template <typename TStateId, typename TEvent>
    struct transition_key_hash {
        std::size_t operator()(const transition_key<TStateId, TEvent>& key) const {
            std::size_t seed = 0;
            seed ^= std::hash<TStateId>{}(key.from) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= std::hash<TEvent>{}(key.event) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

    template <typename TStateId, typename TEvent>
    struct transition_key_equal {
        bool operator()(const transition_key<TStateId, TEvent>& lhs,
                        const transition_key<TStateId, TEvent>& rhs) const {
            return lhs.from == rhs.from && lhs.event == rhs.event;
        }
    };
} // namespace detail

// Deterministic finite state machine with explicit transitions and optional callbacks.
template <typename TStateId, typename TEvent>
class state_machine {
public:
    using state_id = TStateId;
    using event_type = TEvent;
    using transition_callback = std::function<void(const state_id&, const state_id&, const event_type&)>;

    explicit state_machine(state_id initial)
        : current_(std::move(initial)) {
    }

    bool add_transition(state_id from, event_type event, state_id to) {
        detail::transition_key<state_id, event_type> key{std::move(from), std::move(event)};
        auto [slot, inserted] = transitions_.emplace(std::move(key), std::move(to));
        if (!inserted) {
            slot->second = std::move(to);
        }
        return inserted;
    }

    void on_transition(transition_callback callback) {
        transition_callback_ = std::move(callback);
    }

    const state_id& current_state() const noexcept {
        return current_;
    }

    core::result<state_id, std::string> dispatch(const event_type& event) {
        detail::transition_key<state_id, event_type> key{current_, event};
        auto it = transitions_.find(key);
        if (it == transitions_.end()) {
            return core::result<state_id, std::string>::err("transition not found");
        }

        state_id previous = current_;
        current_ = it->second;

        if (transition_callback_) {
            transition_callback_(previous, current_, event);
        }

        return core::result<state_id, std::string>::ok(current_);
    }

private:
    using key_type = detail::transition_key<state_id, event_type>;
    using map_type = std::unordered_map<key_type, state_id,
                                        detail::transition_key_hash<state_id, event_type>,
                                        detail::transition_key_equal<state_id, event_type>>;

    map_type transitions_;
    state_id current_;
    transition_callback transition_callback_;
};

} // namespace basicpp::state

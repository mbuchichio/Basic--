#pragma once

#include <utility>

namespace basicpp::state {

// Lightweight wrapper that carries an identifier for strongly-typed states.
// Extend or replace as needed in client code; the state machine only requires
// that ids are hashable and comparable.
template <typename TStateId>
class state {
public:
    using id_type = TStateId;

    explicit state(TStateId id)
        : id_(std::move(id)) {
    }

    const id_type& id() const noexcept {
        return id_;
    }

private:
    id_type id_;
};

} // namespace basicpp::state

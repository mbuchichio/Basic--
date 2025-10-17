#pragma once

#include <string>

#include <basicpp/core/result.hpp>

namespace basicpp::command {

// Base interface for commands operating on an arbitrary context value.
// Context is passed by const reference; implementations can provide richer wrappers as needed.
template <typename TContext>
class command {
public:
    using context_type = TContext;
    using result_type = core::result<void, std::string>;

    virtual ~command() = default;
    virtual result_type execute(const context_type& context) = 0;
};

} // namespace basicpp::command

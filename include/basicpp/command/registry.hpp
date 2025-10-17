#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <basicpp/core/result.hpp>

namespace basicpp::command {

// Registry that maps string identifiers to command handlers returning basicpp::core::result.
// Handlers are stored as std::function to keep integration friction low.
template <typename TResult, typename... TArgs>
class registry {
public:
    using handler_result = core::result<TResult, std::string>;
    using handler_type = std::function<handler_result(TArgs...)>;

    bool register_handler(std::string key, handler_type handler) {
        auto it = handlers_.find(key);
        if (it == handlers_.end()) {
            handlers_.emplace(std::move(key), std::move(handler));
            return true;
        }

        it->second = std::move(handler);
        return false;
    }

    void unregister_handler(const std::string& key) {
        handlers_.erase(key);
    }

    bool contains(const std::string& key) const {
        return handlers_.find(key) != handlers_.end();
    }

    handler_result dispatch(const std::string& key, TArgs... args) const {
        auto it = handlers_.find(key);
        if (it == handlers_.end()) {
            return handler_result::err("command not found: " + key);
        }
        return it->second(std::forward<TArgs>(args)...);
    }

    std::vector<std::string> keys() const {
        std::vector<std::string> result;
        result.reserve(handlers_.size());
        for (const auto& [key, _] : handlers_) {
            result.push_back(key);
        }
        return result;
    }

    std::size_t size() const noexcept {
        return handlers_.size();
    }

private:
    std::unordered_map<std::string, handler_type> handlers_;
};

} // namespace basicpp::command

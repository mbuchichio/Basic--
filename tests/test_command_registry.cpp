#include <stdexcept>
#include <string>

#include <basicpp/command/registry.hpp>
#include <basicpp/testing/selftest.hpp>

using registry_t = basicpp::command::registry<int>;

BASICPP_TEST(CommandRegistryDispatchesRegisteredCommand) {
    registry_t registry;
    registry.register_handler("answer", [] {
        return registry_t::handler_result::ok(42);
    });

    auto result = registry.dispatch("answer");
    if (!result) {
        throw std::runtime_error("expected handler to succeed");
    }

    if (result.value() != 42) {
        throw std::runtime_error("unexpected handler value");
    }
}

BASICPP_TEST(CommandRegistryFailsWhenCommandMissing) {
    registry_t registry;
    auto result = registry.dispatch("missing");
    if (result) {
        throw std::runtime_error("dispatch should not succeed");
    }
}

BASICPP_TEST_MAIN()

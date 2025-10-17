#include <stdexcept>
#include <string>

#include <basicpp/state/state_machine.hpp>
#include <basicpp/testing/selftest.hpp>

using state_machine_t = basicpp::state::state_machine<std::string, std::string>;

BASICPP_TEST(StateMachineTransitionsWhenMatchExists) {
    state_machine_t machine{"idle"};
    machine.add_transition("idle", "start", "running");

    auto result = machine.dispatch("start");
    if (!result) {
        throw std::runtime_error("expected transition to succeed");
    }

    if (machine.current_state() != "running") {
        throw std::runtime_error("state machine did not advance");
    }
}

BASICPP_TEST(StateMachineFailsWhenTransitionMissing) {
    state_machine_t machine{"idle"};
    auto result = machine.dispatch("stop");
    if (result) {
        throw std::runtime_error("dispatch should have failed");
    }
}

BASICPP_TEST_MAIN()
